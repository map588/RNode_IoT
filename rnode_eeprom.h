#ifndef RNODE_EEPROM_H
#define RNODE_EEPROM_H


#if !HAS_EEPROM && (MCU_VARIANT == MCU_NRF52)
bool eeprom_begin() {
  InternalFS.begin();

  file.open(EEPROM_FILE, FILE_O_READ);
  if (!file) {
    if (file.open(EEPROM_FILE, FILE_O_WRITE)) {
      for (uint32_t mapped_addr = 0; mapped_addr < EEPROM_SIZE; mapped_addr++) {
        file.seek(mapped_addr);
        file.write(0xFF);
      }
      eeprom_flush();
      return true;
    } else {
      return false;
    }
  } else {
    file.close();
    file.open(EEPROM_FILE, FILE_O_WRITE);
    return true;
  }
}

inline uint8_t eeprom_read(uint32_t mapped_addr) {
  uint8_t byte;
  void *byte_ptr = &byte;
  file.seek(mapped_addr);
  file.read(byte_ptr, 1);
  return byte;
}
#elif HAS_EEPROM
inline uint8_t eeprom_read(uint32_t mapped_addr) {
  return EEPROM.read(mapped_addr);
}
#else
inline uint8_t eeprom_read(uint32_t mapped_addr){
  return 0;
}
#endif

bool eeprom_info_locked() {
  uint8_t lock_byte = eeprom_read(eeprom_addr(ADDR_INFO_LOCK));
  if (lock_byte == INFO_LOCK_BYTE) {
    return true;
  } else {
    return false;
  }
}

void eeprom_dump_info() {
  for (int addr = ADDR_PRODUCT; addr <= ADDR_INFO_LOCK; addr++) {
    uint8_t byte = eeprom_read(eeprom_addr(addr));
    escaped_serial_write(byte);
  }
}

void eeprom_dump_config() {
  for (int addr = ADDR_CONF_SF; addr <= ADDR_CONF_OK; addr++) {
    uint8_t byte = eeprom_read(eeprom_addr(addr));
    escaped_serial_write(byte);
  }
}

void eeprom_dump_all() {
  for (int addr = 0; addr < EEPROM_RESERVED; addr++) {
    uint8_t byte = eeprom_read(eeprom_addr(addr));
    escaped_serial_write(byte);
  }
}

void kiss_dump_eeprom() {
  serial_write(FEND);
  serial_write(CMD_ROM_READ);
  eeprom_dump_all();
  serial_write(FEND);
}

#if !HAS_EEPROM && MCU_VARIANT == MCU_NRF52
void eeprom_flush() {
  file.close();
  file.open(EEPROM_FILE, FILE_O_WRITE);
  written_bytes = 0;
}
#endif

void eeprom_update(int mapped_addr, uint8_t byte) {
#if MCU_VARIANT == MCU_1284P || MCU_VARIANT == MCU_2560
  EEPROM.update(mapped_addr, byte);
#elif MCU_VARIANT == MCU_ESP32
  if (eeprom_read(mapped_addr) != byte) {
    EEPROM.write(mapped_addr, byte);
    EEPROM.commit();
  }
#elif !HAS_EEPROM && MCU_VARIANT == MCU_NRF52
  // todo: clean up this implementation, writing one byte and syncing
  // each time is really slow, but this is also suboptimal
  uint8_t read_byte;
  void *read_byte_ptr = &read_byte;
  file.seek(mapped_addr);
  file.read(read_byte_ptr, 1);
  file.seek(mapped_addr);
  if (read_byte != byte) {
    file.write(byte);
  }
  written_bytes++;
  eeprom_flush();
#endif
}

void eeprom_write(uint8_t addr, uint8_t byte) {
  if (!eeprom_info_locked() && addr >= 0 && addr < EEPROM_RESERVED) {
    eeprom_update(eeprom_addr(addr), byte);
  } else {
    kiss_indicate_error(ERROR_EEPROM_LOCKED);
  }
}

void eeprom_erase() {
#if !HAS_EEPROM && MCU_VARIANT == MCU_NRF52
  InternalFS.format();
#else
  for (int addr = 0; addr < EEPROM_RESERVED; addr++) {
    eeprom_update(eeprom_addr(addr), 0xFF);
  }
#endif
  hard_reset();
}

bool eeprom_lock_set() {
#if HAS_EEPROM || (MCU_VARIANT == MCU_NRF52)
  if (eeprom_read(eeprom_addr(ADDR_INFO_LOCK)) == INFO_LOCK_BYTE) {
#endif
    return true;
  } else {
    return false;
  }
}

bool eeprom_product_valid() {
#if HAS_EEPROM || (MCU_VARIANT == MCU_NRF52)
  uint8_t rval = eeprom_read(eeprom_addr(ADDR_PRODUCT));
#endif

#if PLATFORM == PLATFORM_AVR
  if (rval == PRODUCT_RNODE || rval == PRODUCT_HMBRW) {
#elif PLATFORM == PLATFORM_ESP32
  if (rval == PRODUCT_RNODE || rval == BOARD_RNODE_NG_20 || rval == BOARD_RNODE_NG_21 || rval == PRODUCT_HMBRW || rval == PRODUCT_TBEAM || rval == PRODUCT_T32_10 || rval == PRODUCT_T32_20 || rval == PRODUCT_T32_21 || rval == PRODUCT_H32_V2 || rval == PRODUCT_H32_V3 || rval == PRODUCT_TDECK_V1 || rval == PRODUCT_TBEAM_S_V1 || rval == PRODUCT_XIAO_S3) {
#elif PLATFORM == PLATFORM_NRF52
  if (rval == PRODUCT_RAK4631 || rval == PRODUCT_HELTEC_T114 || rval == PRODUCT_TECHO || rval == PRODUCT_HMBRW) {
#else
  if (false) {
#endif
    return true;
  } else {
    return false;
  }
}

bool eeprom_model_valid() {
#if HAS_EEPROM || (MCU_VARIANT == MCU_NRF52)
  model = eeprom_read(eeprom_addr(ADDR_MODEL));
#endif
#if BOARD_MODEL == BOARD_RNODE
  if (model == MODEL_A4 || model == MODEL_A9 || model == MODEL_FF || model == MODEL_FE) {
#elif BOARD_MODEL == BOARD_RNODE_NG_20
  if (model == MODEL_A3 || model == MODEL_A8) {
#elif BOARD_MODEL == BOARD_RNODE_NG_21
  if (model == MODEL_A2 || model == MODEL_A7) {
#elif BOARD_MODEL == BOARD_T3S3
  if (model == MODEL_A1 || model == MODEL_A6 || model == MODEL_A5 || model == MODEL_AA || model == MODEL_AC) {
#elif BOARD_MODEL == BOARD_HMBRW
  if (model == MODEL_FF || model == MODEL_FE) {
#elif BOARD_MODEL == BOARD_TBEAM
  if (model == MODEL_E4 || model == MODEL_E9 || model == MODEL_E3 || model == MODEL_E8) {
#elif BOARD_MODEL == BOARD_TDECK
  if (model == MODEL_D4 || model == MODEL_D9) {
#elif BOARD_MODEL == BOARD_TECHO
  if (model == MODEL_16 || model == MODEL_17) {
#elif BOARD_MODEL == BOARD_TBEAM_S_V1
  if (model == MODEL_DB || model == MODEL_DC) {
#elif BOARD_MODEL == BOARD_XIAO_S3
  if (model == MODEL_DD || model == MODEL_DE) {
#elif BOARD_MODEL == BOARD_LORA32_V1_0
  if (model == MODEL_BA || model == MODEL_BB) {
#elif BOARD_MODEL == BOARD_LORA32_V2_0
  if (model == MODEL_B3 || model == MODEL_B8) {
#elif BOARD_MODEL == BOARD_LORA32_V2_1
  if (model == MODEL_B4 || model == MODEL_B9) {
#elif BOARD_MODEL == BOARD_HELTEC32_V2
  if (model == MODEL_C4 || model == MODEL_C9) {
#elif BOARD_MODEL == BOARD_HELTEC32_V3
  if (model == MODEL_C5 || model == MODEL_CA) {
#elif BOARD_MODEL == BOARD_HELTEC_T114
  if (model == MODEL_C6 || model == MODEL_C7) {
#elif BOARD_MODEL == BOARD_RAK4631
  if (model == MODEL_11 || model == MODEL_12) {
#elif BOARD_MODEL == BOARD_HUZZAH32
  if (model == MODEL_FF) {
#elif BOARD_MODEL == BOARD_GENERIC_ESP32
  if (model == MODEL_FF || model == MODEL_FE) {
#else
  if (false) {
#endif
    return true;
  } else {
    return false;
  }
}

bool eeprom_hwrev_valid() {
#if HAS_EEPROM || (MCU_VARIANT == MCU_NRF52)
  hwrev = eeprom_read(eeprom_addr(ADDR_HW_REV));
#endif
  if (hwrev != 0x00 && hwrev != 0xFF) {
    return true;
  } else {
    return false;
  }
}

bool eeprom_checksum_valid() {
  char *data = (char *)malloc(CHECKSUMMED_SIZE);
  for (uint8_t i = 0; i < CHECKSUMMED_SIZE; i++) {
#if HAS_EEPROM || (MCU_VARIANT == MCU_NRF52)
    char byte = eeprom_read(eeprom_addr(i));
#endif
    data[i] = byte;
  }

  unsigned char *hash = MD5::make_hash(data, CHECKSUMMED_SIZE);
  bool checksum_valid = true;
  for (uint8_t i = 0; i < 16; i++) {
#if HAS_EEPROM || (MCU_VARIANT == MCU_NRF52)
    uint8_t stored_chk_byte = eeprom_read(eeprom_addr(ADDR_CHKSUM + i));
#endif
    uint8_t calced_chk_byte = (uint8_t)hash[i];
    if (stored_chk_byte != calced_chk_byte) {
      checksum_valid = false;
    }
  }

  free(hash);
  free(data);
  return checksum_valid;
}

void bt_conf_save(bool is_enabled) {
  if (is_enabled) {
    eeprom_update(eeprom_addr(ADDR_CONF_BT), BT_ENABLE_BYTE);
#if !HAS_EEPROM && MCU_VARIANT == MCU_NRF52
    // have to do a flush because we're only writing 1 byte and it syncs after 8
    eeprom_flush();
#endif
  } else {
    eeprom_update(eeprom_addr(ADDR_CONF_BT), 0x00);
#if !HAS_EEPROM && MCU_VARIANT == MCU_NRF52
    // have to do a flush because we're only writing 1 byte and it syncs after 8
    eeprom_flush();
#endif
  }
}

void di_conf_save(uint8_t dint) {
  eeprom_update(eeprom_addr(ADDR_CONF_DINT), dint);
}

void da_conf_save(uint8_t dadr) {
  eeprom_update(eeprom_addr(ADDR_CONF_DADR), dadr);
}

void db_conf_save(uint8_t val) {
#if HAS_DISPLAY
  if (val == 0x00) {
    display_blanking_enabled = false;
  } else {
    display_blanking_enabled = true;
    display_blanking_timeout = val * 1000;
  }
  eeprom_update(eeprom_addr(ADDR_CONF_BSET), CONF_OK_BYTE);
  eeprom_update(eeprom_addr(ADDR_CONF_DBLK), val);
#endif
}

void drot_conf_save(uint8_t val) {
#if HAS_DISPLAY
  if (val >= 0x00 and val <= 0x03) {
    eeprom_update(eeprom_addr(ADDR_CONF_DROT), val);
    hard_reset();
  }
#endif
}

void dia_conf_save(uint8_t val) {
  if (val > 0x00) {
    eeprom_update(eeprom_addr(ADDR_CONF_DIA), 0x01);
  } else {
    eeprom_update(eeprom_addr(ADDR_CONF_DIA), 0x00);
  }
  hard_reset();
}

void np_int_conf_save(uint8_t p_int) {
  eeprom_update(eeprom_addr(ADDR_CONF_PSET), CONF_OK_BYTE);
  eeprom_update(eeprom_addr(ADDR_CONF_PINT), p_int);
}


bool eeprom_have_conf() {
  return (eeprom_read(eeprom_addr(ADDR_CONF_OK)) == CONF_OK_BYTE);
}

void eeprom_conf_load() {
  if (eeprom_have_conf()) {
    lora_sf = eeprom_read(eeprom_addr(ADDR_CONF_SF));
    lora_cr = eeprom_read(eeprom_addr(ADDR_CONF_CR));
    lora_txp = eeprom_read(eeprom_addr(ADDR_CONF_TXP));
    lora_freq = (uint32_t)eeprom_read(eeprom_addr(ADDR_CONF_FREQ) + 0x00) << 24 | (uint32_t)eeprom_read(eeprom_addr(ADDR_CONF_FREQ) + 0x01) << 16 | (uint32_t)eeprom_read(eeprom_addr(ADDR_CONF_FREQ) + 0x02) << 8 | (uint32_t)eeprom_read(eeprom_addr(ADDR_CONF_FREQ) + 0x03);
    lora_bw = (uint32_t)eeprom_read(eeprom_addr(ADDR_CONF_BW) + 0x00) << 24 | (uint32_t)eeprom_read(eeprom_addr(ADDR_CONF_BW) + 0x01) << 16 | (uint32_t)eeprom_read(eeprom_addr(ADDR_CONF_BW) + 0x02) << 8 | (uint32_t)eeprom_read(eeprom_addr(ADDR_CONF_BW) + 0x03);
  }
}

void eeprom_conf_save() {
  if (hw_ready && radio_online) {
    eeprom_update(eeprom_addr(ADDR_CONF_SF), lora_sf);
    eeprom_update(eeprom_addr(ADDR_CONF_CR), lora_cr);
    eeprom_update(eeprom_addr(ADDR_CONF_TXP), lora_txp);

    eeprom_update(eeprom_addr(ADDR_CONF_BW) + 0x00, lora_bw >> 24);
    eeprom_update(eeprom_addr(ADDR_CONF_BW) + 0x01, lora_bw >> 16);
    eeprom_update(eeprom_addr(ADDR_CONF_BW) + 0x02, lora_bw >> 8);
    eeprom_update(eeprom_addr(ADDR_CONF_BW) + 0x03, lora_bw);

    eeprom_update(eeprom_addr(ADDR_CONF_FREQ) + 0x00, lora_freq >> 24);
    eeprom_update(eeprom_addr(ADDR_CONF_FREQ) + 0x01, lora_freq >> 16);
    eeprom_update(eeprom_addr(ADDR_CONF_FREQ) + 0x02, lora_freq >> 8);
    eeprom_update(eeprom_addr(ADDR_CONF_FREQ) + 0x03, lora_freq);

    eeprom_update(eeprom_addr(ADDR_CONF_OK), CONF_OK_BYTE);
    led_indicate_info(10);
  } else {
    led_indicate_warning(10);
  }
}

void eeprom_conf_delete() {
  eeprom_update(eeprom_addr(ADDR_CONF_OK), 0x00);
}

void unlock_rom() {
  led_indicate_error(50);
  eeprom_erase();
}

#endif /*RNODE_EEPROM_H*/