// Copyright (C) 2024, Mark Qvist

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Config.h"

#if HAS_EEPROM
    #include <EEPROM.h>
#elif PLATFORM == PLATFORM_NRF52
		#include <hal/nrf_rng.h>
    #include <Adafruit_LittleFS.h>
    #include <InternalFileSystem.h>
    using namespace Adafruit_LittleFS_Namespace;
    #define EEPROM_FILE "eeprom"
    bool file_exists = false;
    int written_bytes = 4;
    File file(InternalFS);
#endif
#include <stddef.h>

#if MODEM == SX1262
#include "Lora_Modules/sx126x.h"
sx126x *LoRa = &sx126x_modem;
#elif MODEM == SX1276 || MODEM == SX1278
#include "Lora_Modules/sx127x.h"
sx127x *LoRa = &sx127x_modem;
#elif MODEM == SX1280
#include "Lora_Modules/sx128x.h"
sx128x *LoRa = &sx128x_modem;
#endif

#include "ROM.h"
#include "Framing.h"
#include "MD5.h"

#define eeprom_addr(a) (a+EEPROM_OFFSET)

// Consolidated eeprom_read with an inline hint to the compiler when using <EEPROM.h> variant
#if HAS_EEPROM || (!HAS_EEPROM && MCU_VARIANT == MCU_NRF52)
inline uint8_t eeprom_read(uint32_t mapped_addr);
#endif

#if HAS_DISPLAY == true
#include "Display.h"
#else
void display_unblank() {}
bool display_blanked = false;
#endif

#if HAS_BLUETOOTH == true || HAS_BLE == true
void kiss_indicate_btpin();
#include "Bluetooth.h"
#endif

#if HAS_PMU == true
#include "Power.h"
#endif

#if HAS_INPUT == true
#include "Input.h"
#endif

#if MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
#include "Device.h"
#endif
#if MCU_VARIANT == MCU_ESP32
//https://github.com/espressif/esp-idf/issues/8855
#include "hal/wdt_hal.h"
#define ISR_VECT IRAM_ATTR
#else
#define ISR_VECT
#endif

#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
#include <avr/wdt.h>
#include <util/atomic.h>
#endif

uint8_t boot_vector = 0x00;

#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
uint8_t OPTIBOOT_MCUSR __attribute__((section(".noinit")));
void resetFlagsInit(void) __attribute__((naked)) __attribute__((used)) __attribute__((section(".init0")));
void resetFlagsInit(void) {
	__asm__ __volatile__("sts %0, r2\n"
	                     : "=m"(OPTIBOOT_MCUSR)
	                     :);
}
#elif MCU_VARIANT == MCU_ESP32
// TODO: Get ESP32 boot flags
#elif MCU_VARIANT == MCU_NRF52
// TODO: Get NRF52 boot flags
#endif

#if MCU_VARIANT == MCU_NRF52
unsigned long get_rng_seed() {
	nrf_rng_error_correction_enable(NRF_RNG);
	nrf_rng_shorts_disable(NRF_RNG, NRF_RNG_SHORT_VALRDY_STOP_MASK);
	nrf_rng_task_trigger(NRF_RNG, NRF_RNG_TASK_START);
	while (!nrf_rng_event_check(NRF_RNG, NRF_RNG_EVENT_VALRDY))
		;
	uint8_t rb_a = nrf_rng_random_value_get(NRF_RNG);
	nrf_rng_event_clear(NRF_RNG, NRF_RNG_EVENT_VALRDY);
	while (!nrf_rng_event_check(NRF_RNG, NRF_RNG_EVENT_VALRDY))
		;
	uint8_t rb_b = nrf_rng_random_value_get(NRF_RNG);
	nrf_rng_event_clear(NRF_RNG, NRF_RNG_EVENT_VALRDY);
	while (!nrf_rng_event_check(NRF_RNG, NRF_RNG_EVENT_VALRDY))
		;
	uint8_t rb_c = nrf_rng_random_value_get(NRF_RNG);
	nrf_rng_event_clear(NRF_RNG, NRF_RNG_EVENT_VALRDY);
	while (!nrf_rng_event_check(NRF_RNG, NRF_RNG_EVENT_VALRDY))
		;
	uint8_t rb_d = nrf_rng_random_value_get(NRF_RNG);
	nrf_rng_event_clear(NRF_RNG, NRF_RNG_EVENT_VALRDY);
	nrf_rng_task_trigger(NRF_RNG, NRF_RNG_TASK_STOP);
	return rb_a << 24 | rb_b << 16 | rb_c << 8 | rb_d;
}
#endif


#include "Utilities/Led.h"
#include "Utilities/Serial_Comm.h"
#include "Utilities/Kiss.h"
#include "Utilities/rnode_eeprom.h"


typedef struct FIFOBuffer {
	unsigned char *begin;
	unsigned char *end;
	unsigned char *volatile head;
	unsigned char *volatile tail;
} FIFOBuffer;

inline bool fifo_isempty(const FIFOBuffer *f) {
	return f->head == f->tail;
}

inline bool fifo_isfull(const FIFOBuffer *f) {
	return ((f->head == f->begin) && (f->tail == f->end)) || (f->tail == f->head - 1);
}

inline void fifo_push(FIFOBuffer *f, unsigned char c) {
	*(f->tail) = c;

	if (f->tail == f->end) {
		f->tail = f->begin;
	} else {
		f->tail++;
	}
}

inline unsigned char fifo_pop(FIFOBuffer *f) {
	if (f->head == f->end) {
		f->head = f->begin;
		return *(f->end);
	} else {
		return *(f->head++);
	}
}

inline void fifo_flush(FIFOBuffer *f) {
	f->head = f->tail;
}

#if MCU_VARIANT != MCU_ESP32 && MCU_VARIANT != MCU_NRF52
static inline bool fifo_isempty_locked(const FIFOBuffer *f) {
	bool result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		result = fifo_isempty(f);
	}
	return result;
}

static inline bool fifo_isfull_locked(const FIFOBuffer *f) {
	bool result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		result = fifo_isfull(f);
	}
	return result;
}

static inline void fifo_push_locked(FIFOBuffer *f, unsigned char c) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		fifo_push(f, c);
	}
}
#endif

inline void fifo_init(FIFOBuffer *f, unsigned char *buffer, size_t size) {
	f->head = f->tail = f->begin = buffer;
	f->end = buffer + size;
}

inline size_t fifo_len(FIFOBuffer *f) {
	return f->end - f->begin;
}

typedef struct FIFOBuffer16 {
	uint16_t *begin;
	uint16_t *end;
	uint16_t *volatile head;
	uint16_t *volatile tail;
} FIFOBuffer16;

inline bool fifo16_isempty(const FIFOBuffer16 *f) {
	return f->head == f->tail;
}

inline bool fifo16_isfull(const FIFOBuffer16 *f) {
	return ((f->head == f->begin) && (f->tail == f->end)) || (f->tail == f->head - 1);
}

inline void fifo16_push(FIFOBuffer16 *f, uint16_t c) {
	*(f->tail) = c;

	if (f->tail == f->end) {
		f->tail = f->begin;
	} else {
		f->tail++;
	}
}

inline uint16_t fifo16_pop(FIFOBuffer16 *f) {
	if (f->head == f->end) {
		f->head = f->begin;
		return *(f->end);
	} else {
		return *(f->head++);
	}
}

inline void fifo16_flush(FIFOBuffer16 *f) {
	f->head = f->tail;
}

#if MCU_VARIANT != MCU_ESP32 && MCU_VARIANT != MCU_NRF52
static inline bool fifo16_isempty_locked(const FIFOBuffer16 *f) {
	bool result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		result = fifo16_isempty(f);
	}

	return result;
}
#endif


inline void fifo16_init(FIFOBuffer16 *f, uint16_t *buffer, uint16_t size) {
	f->head = f->tail = f->begin = buffer;
	f->end = buffer + size;
}

inline uint16_t fifo16_len(FIFOBuffer16 *f) {
	return (f->end - f->begin);
}
