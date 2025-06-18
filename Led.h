#ifndef LED_H
#define LED_H


#if HAS_NP == true
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, pin_np, NEO_GRB + NEO_KHZ800);

uint8_t npr = 0;
uint8_t npg = 0;
uint8_t npb = 0;
float npi = NP_M;
bool pixels_started = false;

void led_set_intensity(uint8_t intensity) {
	npi = (float)intensity / 255.0;
}

void led_init() {
#if BOARD_MODEL == BOARD_HELTEC_T114
	// Enable vext power supply to neopixel
	pinMode(PIN_VEXT_EN, OUTPUT);
	digitalWrite(PIN_VEXT_EN, HIGH);
#endif

	if (eeprom_read(eeprom_addr(ADDR_CONF_PSET)) == CONF_OK_BYTE) {
		uint8_t int_val = eeprom_read(eeprom_addr(ADDR_CONF_PINT));
		led_set_intensity(int_val);
	}
}

void npset(uint8_t r, uint8_t g, uint8_t b) {
	if (pixels_started != true) {
		pixels.begin();
		pixels_started = true;
	}

	if (r != npr || g != npg || b != npb) {
		npr = r;
		npg = g;
		npb = b;
		pixels.setPixelColor(0, pixels.Color(npr * npi, npg * npi, npb * npi));
		pixels.show();
	}
}

void boot_seq() {
	uint8_t rs[] = { 0x00, 0x00, 0x00 };
	uint8_t gs[] = { 0x10, 0x08, 0x00 };
	uint8_t bs[] = { 0x00, 0x08, 0x10 };
	for (int i = 0; i < 1 * sizeof(rs); i++) {
		npset(rs[i % sizeof(rs)], gs[i % sizeof(gs)], bs[i % sizeof(bs)]);
		delay(33);
		npset(0x00, 0x00, 0x00);
		delay(66);
	}
}
#else
void boot_seq() {}
#endif

#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#elif MCU_VARIANT == MCU_ESP32
#if HAS_NP == true
void led_rx_on() {
	npset(0, 0, 0xFF);
}
void led_rx_off() {
	npset(0, 0, 0);
}
void led_tx_on() {
	npset(0xFF, 0x50, 0x00);
}
void led_tx_off() {
	npset(0, 0, 0);
}
void led_id_on() {
	npset(0x90, 0, 0x70);
}
void led_id_off() {
	npset(0, 0, 0);
}
#elif BOARD_MODEL == BOARD_RNODE_NG_20
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_RNODE_NG_21
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_T3S3
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_TBEAM
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, LOW);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_TDECK
void led_rx_on() {}
void led_rx_off() {}
void led_tx_on() {}
void led_tx_off() {}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_TBEAM_S_V1
void led_rx_on() {}
void led_rx_off() {}
void led_tx_on() {}
void led_tx_off() {}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_LORA32_V1_0
#if defined(EXTERNAL_LEDS)
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#else
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#endif
#elif BOARD_MODEL == BOARD_LORA32_V2_0
#if defined(EXTERNAL_LEDS)
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#else
void led_rx_on() {
	digitalWrite(pin_led_rx, LOW);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, LOW);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_id_on() {}
void led_id_off() {}
#endif
#elif BOARD_MODEL == BOARD_HELTEC32_V2
#if defined(EXTERNAL_LEDS)
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#else
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#endif
#elif BOARD_MODEL == BOARD_HELTEC32_V3
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_LORA32_V2_1
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_XIAO_S3
void led_rx_on() {
	digitalWrite(pin_led_rx, LED_ON);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LED_OFF);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, LED_ON);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LED_OFF);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_HUZZAH32
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_GENERIC_ESP32
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#endif
#elif MCU_VARIANT == MCU_NRF52
#if HAS_NP == true
void led_rx_on() {
	npset(0, 0, 0xFF);
}
void led_rx_off() {
	npset(0, 0, 0);
}
void led_tx_on() {
	npset(0xFF, 0x50, 0x00);
}
void led_tx_off() {
	npset(0, 0, 0);
}
void led_id_on() {
	npset(0x90, 0, 0x70);
}
void led_id_off() {
	npset(0, 0, 0);
}
#elif BOARD_MODEL == BOARD_RAK4631
void led_rx_on() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LOW);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LOW);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_HELTEC_T114
// Heltec T114 pulls pins LOW to turn on
void led_rx_on() {
	digitalWrite(pin_led_rx, LOW);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, HIGH);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, LOW);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, HIGH);
}
void led_id_on() {}
void led_id_off() {}
#elif BOARD_MODEL == BOARD_TECHO
void led_rx_on() {
	digitalWrite(pin_led_rx, LED_ON);
}
void led_rx_off() {
	digitalWrite(pin_led_rx, LED_OFF);
}
void led_tx_on() {
	digitalWrite(pin_led_tx, LED_ON);
}
void led_tx_off() {
	digitalWrite(pin_led_tx, LED_OFF);
}
void led_id_on() {}
void led_id_off() {}
#endif
#endif

void hard_reset(void) {
#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
	wdt_enable(WDTO_15MS);
	while (true) {
		led_tx_on();
		led_rx_off();
	}
#elif MCU_VARIANT == MCU_ESP32
	ESP.restart();
#elif MCU_VARIANT == MCU_NRF52
	NVIC_SystemReset();
#endif
}

// LED Indication: Error
void led_indicate_error(int cycles) {
#if HAS_NP == true
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		npset(0xFF, 0x00, 0x00);
		delay(100);
		npset(0xFF, 0x50, 0x00);
		delay(100);
		if (!forever) cycles--;
	}
	npset(0, 0, 0);
#else
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		digitalWrite(pin_led_rx, HIGH);
		digitalWrite(pin_led_tx, LOW);
		delay(100);
		digitalWrite(pin_led_rx, LOW);
		digitalWrite(pin_led_tx, HIGH);
		delay(100);
		if (!forever) cycles--;
	}
	led_rx_off();
	led_tx_off();
#endif
}

// LED Indication: Airtime Lock
void led_indicate_airtime_lock() {
#if HAS_NP == true
	npset(32, 0, 2);
#endif
}

// LED Indication: Boot Error
void led_indicate_boot_error() {
#if HAS_NP == true
	while (true) {
		npset(0xFF, 0xFF, 0xFF);
	}
#else
	while (true) {
		led_tx_on();
		led_rx_off();
		delay(10);
		led_rx_on();
		led_tx_off();
		delay(5);
	}
#endif
}

// LED Indication: Warning
void led_indicate_warning(int cycles) {
#if HAS_NP == true
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		npset(0xFF, 0x50, 0x00);
		delay(100);
		npset(0x00, 0x00, 0x00);
		delay(100);
		if (!forever) cycles--;
	}
	npset(0, 0, 0);
#else
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	digitalWrite(pin_led_tx, HIGH);
	while (cycles > 0) {
		led_tx_off();
		delay(100);
		led_tx_on();
		delay(100);
		if (!forever) cycles--;
	}
	led_tx_off();
#endif
}

// LED Indication: Info
#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
void led_indicate_info(int cycles) {
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		led_rx_off();
		delay(100);
		led_rx_on();
		delay(100);
		if (!forever) cycles--;
	}
	led_rx_off();
}
#elif MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
#if HAS_NP == true
void led_indicate_info(int cycles) {
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		npset(0x00, 0x00, 0xFF);
		delay(100);
		npset(0x00, 0x00, 0x00);
		delay(100);
		if (!forever) cycles--;
	}
	npset(0, 0, 0);
}
#elif BOARD_MODEL == BOARD_LORA32_V2_1
void led_indicate_info(int cycles) {
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		led_rx_off();
		delay(100);
		led_rx_on();
		delay(100);
		if (!forever) cycles--;
	}
	led_rx_off();
}
#elif BOARD_MODEL == BOARD_LORA32_V2_0
void led_indicate_info(int cycles) {
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		led_rx_off();
		delay(100);
		led_rx_on();
		delay(100);
		if (!forever) cycles--;
	}
	led_rx_off();
}
#elif BOARD_MODEL == BOARD_TECHO
void led_indicate_info(int cycles) {
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		led_rx_off();
		delay(100);
		led_rx_on();
		delay(100);
		if (!forever) cycles--;
	}
	led_rx_off();
}
#else
void led_indicate_info(int cycles) {
	bool forever = (cycles == 0) ? true : false;
	cycles = forever ? 1 : cycles;
	while (cycles > 0) {
		led_tx_off();
		delay(100);
		led_tx_on();
		delay(100);
		if (!forever) cycles--;
	}
	led_tx_off();
}
#endif
#endif


unsigned long led_standby_ticks = 0;
#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
uint8_t led_standby_min = 1;
uint8_t led_standby_max = 40;
unsigned long led_standby_wait = 11000;

#elif MCU_VARIANT == MCU_ESP32

#if HAS_NP == true
int led_standby_lng = 200;
int led_standby_cut = 100;
int led_standby_min = 0;
int led_standby_max = 375 + led_standby_lng;
int led_notready_min = 0;
int led_notready_max = led_standby_max;
int led_notready_value = led_notready_min;
int8_t led_notready_direction = 0;
unsigned long led_notready_ticks = 0;
unsigned long led_standby_wait = 350;
unsigned long led_notready_wait = 200;

#else
uint8_t led_standby_min = 200;
uint8_t led_standby_max = 255;
uint8_t led_notready_min = 0;
uint8_t led_notready_max = 255;
uint8_t led_notready_value = led_notready_min;
int8_t led_notready_direction = 0;
unsigned long led_notready_ticks = 0;
unsigned long led_standby_wait = 1768;
unsigned long led_notready_wait = 150;
#endif

#elif MCU_VARIANT == MCU_NRF52
int led_standby_lng = 200;
int led_standby_cut = 100;
uint8_t led_standby_min = 200;
uint8_t led_standby_max = 255;
uint8_t led_notready_min = 0;
uint8_t led_notready_max = 255;
uint8_t led_notready_value = led_notready_min;
int8_t led_notready_direction = 0;
unsigned long led_notready_ticks = 0;
unsigned long led_standby_wait = 1768;
unsigned long led_notready_wait = 150;
#endif

unsigned long led_standby_value = led_standby_min;
int8_t led_standby_direction = 0;

#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
void led_indicate_standby() {
	led_standby_ticks++;
	if (led_standby_ticks > led_standby_wait) {
		led_standby_ticks = 0;
		if (led_standby_value <= led_standby_min) {
			led_standby_direction = 1;
		} else if (led_standby_value >= led_standby_max) {
			led_standby_direction = -1;
		}
		led_standby_value += led_standby_direction;
		analogWrite(pin_led_rx, led_standby_value);
		led_tx_off();
	}
}

#elif MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
#if HAS_NP == true
void led_indicate_standby() {
	led_standby_ticks++;

	if (led_standby_ticks > led_standby_wait) {
		led_standby_ticks = 0;

		if (led_standby_value <= led_standby_min) {
			led_standby_direction = 1;
		} else if (led_standby_value >= led_standby_max) {
			led_standby_direction = -1;
		}

		uint8_t led_standby_intensity;
		led_standby_value += led_standby_direction;
		int led_standby_ti = led_standby_value - led_standby_lng;

		if (led_standby_ti < 0) {
			led_standby_intensity = 0;
		} else if (led_standby_ti > led_standby_cut) {
			led_standby_intensity = led_standby_cut;
		} else {
			led_standby_intensity = led_standby_ti;
		}
		npset(led_standby_intensity / 3, led_standby_intensity / 3, led_standby_intensity / 3);
	}
}

#else
void led_indicate_standby() {
	led_standby_ticks++;
	if (led_standby_ticks > led_standby_wait) {
		led_standby_ticks = 0;
		if (led_standby_value <= led_standby_min) {
			led_standby_direction = 1;
		} else if (led_standby_value >= led_standby_max) {
			led_standby_direction = -1;
		}
		led_standby_value += led_standby_direction;
		if (led_standby_value > 253) {
#if BOARD_MODEL == BOARD_TECHO
			led_rx_on();
#else
			led_tx_on();
#endif
		} else {
#if BOARD_MODEL == BOARD_TECHO
			led_rx_off();
#else
			led_tx_off();
#endif
		}
#if BOARD_MODEL == BOARD_LORA32_V2_1
#if defined(EXTERNAL_LEDS)
		led_rx_off();
#endif
#elif BOARD_MODEL == BOARD_LORA32_V2_0
#if defined(EXTERNAL_LEDS)
		led_rx_off();
#endif
#else
		led_rx_off();
#endif
	}
}
#endif
#endif

#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
void led_indicate_not_ready() {
	led_standby_ticks++;
	if (led_standby_ticks > led_standby_wait) {
		led_standby_ticks = 0;
		if (led_standby_value <= led_standby_min) {
			led_standby_direction = 1;
		} else if (led_standby_value >= led_standby_max) {
			led_standby_direction = -1;
		}
		led_standby_value += led_standby_direction;
		analogWrite(pin_led_tx, led_standby_value);
		led_rx_off();
	}
}
#elif MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
#if HAS_NP == true
void led_indicate_not_ready() {
	led_standby_ticks++;

	if (led_standby_ticks > led_notready_wait) {
		led_standby_ticks = 0;

		if (led_standby_value <= led_standby_min) {
			led_standby_direction = 1;
		} else if (led_standby_value >= led_standby_max) {
			led_standby_direction = -1;
		}

		uint8_t led_standby_intensity;
		led_standby_value += led_standby_direction;
		int led_standby_ti = led_standby_value - led_standby_lng;

		if (led_standby_ti < 0) {
			led_standby_intensity = 0;
		} else if (led_standby_ti > led_standby_cut) {
			led_standby_intensity = led_standby_cut;
		} else {
			led_standby_intensity = led_standby_ti;
		}

		npset(led_standby_intensity, 0x00, 0x00);
	}
}
#else
void led_indicate_not_ready() {
	led_notready_ticks++;
	if (led_notready_ticks > led_notready_wait) {
		led_notready_ticks = 0;
		if (led_notready_value <= led_notready_min) {
			led_notready_direction = 1;
		} else if (led_notready_value >= led_notready_max) {
			led_notready_direction = -1;
		}
		led_notready_value += led_notready_direction;
		if (led_notready_value > 128) {
			led_tx_on();
		} else {
			led_tx_off();
		}
#if BOARD_MODEL == BOARD_LORA32_V2_1
#if defined(EXTERNAL_LEDS)
		led_rx_off();
#endif
#elif BOARD_MODEL == BOARD_LORA32_V2_0
#if defined(EXTERNAL_LEDS)
		led_rx_off();
#endif
#else
		led_rx_off();
#endif
	}
}
#endif
#endif
#endif /*LED_H*/