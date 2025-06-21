#ifndef KISS_H
#define KISS_H


void kiss_indicate_reset() {
	serial_write(FEND);
	serial_write(CMD_RESET);
	serial_write(CMD_RESET_BYTE);
	serial_write(FEND);
}

void kiss_indicate_error(uint8_t error_code) {
	serial_write(FEND);
	serial_write(CMD_ERROR);
	serial_write(error_code);
	serial_write(FEND);
}

void kiss_indicate_radiostate() {
	serial_write(FEND);
	serial_write(CMD_RADIO_STATE);
	serial_write(radio_online);
	serial_write(FEND);
}

void kiss_indicate_stat_rx() {
	serial_write(FEND);
	serial_write(CMD_STAT_RX);
	escaped_serial_write(stat_rx >> 24);
	escaped_serial_write(stat_rx >> 16);
	escaped_serial_write(stat_rx >> 8);
	escaped_serial_write(stat_rx);
	serial_write(FEND);
}

void kiss_indicate_stat_tx() {
	serial_write(FEND);
	serial_write(CMD_STAT_TX);
	escaped_serial_write(stat_tx >> 24);
	escaped_serial_write(stat_tx >> 16);
	escaped_serial_write(stat_tx >> 8);
	escaped_serial_write(stat_tx);
	serial_write(FEND);
}

void kiss_indicate_stat_rssi() {
	uint8_t packet_rssi_val = (uint8_t)(last_rssi + rssi_offset);
	serial_write(FEND);
	serial_write(CMD_STAT_RSSI);
	escaped_serial_write(packet_rssi_val);
	serial_write(FEND);
}

void kiss_indicate_stat_snr() {
	serial_write(FEND);
	serial_write(CMD_STAT_SNR);
	escaped_serial_write(last_snr_raw);
	serial_write(FEND);
}

void kiss_indicate_radio_lock() {
	serial_write(FEND);
	serial_write(CMD_RADIO_LOCK);
	serial_write(radio_locked);
	serial_write(FEND);
}

void kiss_indicate_spreadingfactor() {
	serial_write(FEND);
	serial_write(CMD_SF);
	serial_write((uint8_t)lora_sf);
	serial_write(FEND);
}

void kiss_indicate_codingrate() {
	serial_write(FEND);
	serial_write(CMD_CR);
	serial_write((uint8_t)lora_cr);
	serial_write(FEND);
}

void kiss_indicate_implicit_length() {
	serial_write(FEND);
	serial_write(CMD_IMPLICIT);
	serial_write(implicit_l);
	serial_write(FEND);
}

void kiss_indicate_txpower() {
	serial_write(FEND);
	serial_write(CMD_TXPOWER);
	serial_write((uint8_t)lora_txp);
	serial_write(FEND);
}

void kiss_indicate_bandwidth() {
	serial_write(FEND);
	serial_write(CMD_BANDWIDTH);
	escaped_serial_write(lora_bw >> 24);
	escaped_serial_write(lora_bw >> 16);
	escaped_serial_write(lora_bw >> 8);
	escaped_serial_write(lora_bw);
	serial_write(FEND);
}

void kiss_indicate_frequency() {
	serial_write(FEND);
	serial_write(CMD_FREQUENCY);
	escaped_serial_write(lora_freq >> 24);
	escaped_serial_write(lora_freq >> 16);
	escaped_serial_write(lora_freq >> 8);
	escaped_serial_write(lora_freq);
	serial_write(FEND);
}

void kiss_indicate_st_alock() {
	uint16_t at = (uint16_t)(st_airtime_limit * 100 * 100);
	serial_write(FEND);
	serial_write(CMD_ST_ALOCK);
	escaped_serial_write(at >> 8);
	escaped_serial_write(at);
	serial_write(FEND);
}

void kiss_indicate_lt_alock() {
	uint16_t at = (uint16_t)(lt_airtime_limit * 100 * 100);
	serial_write(FEND);
	serial_write(CMD_LT_ALOCK);
	escaped_serial_write(at >> 8);
	escaped_serial_write(at);
	serial_write(FEND);
}

void kiss_indicate_channel_stats() {
#if MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
	uint16_t ats = (uint16_t)(airtime * 100 * 100);
	uint16_t atl = (uint16_t)(longterm_airtime * 100 * 100);
	uint16_t cls = (uint16_t)(total_channel_util * 100 * 100);
	uint16_t cll = (uint16_t)(longterm_channel_util * 100 * 100);
	uint8_t crs = (uint8_t)(current_rssi + rssi_offset);
	uint8_t nfl = (uint8_t)(noise_floor + rssi_offset);
	uint8_t ntf = 0xFF;
	if (interference_detected) { ntf = (uint8_t)(current_rssi + rssi_offset); }
	serial_write(FEND);
	serial_write(CMD_STAT_CHTM);
	escaped_serial_write(ats >> 8);
	escaped_serial_write(ats);
	escaped_serial_write(atl >> 8);
	escaped_serial_write(atl);
	escaped_serial_write(cls >> 8);
	escaped_serial_write(cls);
	escaped_serial_write(cll >> 8);
	escaped_serial_write(cll);
	escaped_serial_write(crs);
	escaped_serial_write(nfl);
	escaped_serial_write(ntf);
	serial_write(FEND);
#endif
}

void kiss_indicate_csma_stats() {
#if MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
	serial_write(FEND);
	serial_write(CMD_STAT_CSMA);
	escaped_serial_write(cw_band);
	escaped_serial_write(cw_min);
	escaped_serial_write(cw_max);
	serial_write(FEND);
#endif
}

void kiss_indicate_phy_stats() {
#if MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
	uint16_t lst = (uint16_t)(lora_symbol_time_ms * 1000);
	uint16_t lsr = (uint16_t)(lora_symbol_rate);
	uint16_t prs = (uint16_t)(lora_preamble_symbols);
	uint16_t prt = (uint16_t)(lora_preamble_time_ms);
	uint16_t cst = (uint16_t)(csma_slot_ms);
	uint16_t dft = (uint16_t)(difs_ms);
	serial_write(FEND);
	serial_write(CMD_STAT_PHYPRM);
	escaped_serial_write(lst >> 8);
	escaped_serial_write(lst);
	escaped_serial_write(lsr >> 8);
	escaped_serial_write(lsr);
	escaped_serial_write(prs >> 8);
	escaped_serial_write(prs);
	escaped_serial_write(prt >> 8);
	escaped_serial_write(prt);
	escaped_serial_write(cst >> 8);
	escaped_serial_write(cst);
	escaped_serial_write(dft >> 8);
	escaped_serial_write(dft);
	serial_write(FEND);
#endif
}

void kiss_indicate_battery() {
#if MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
	serial_write(FEND);
	serial_write(CMD_STAT_BAT);
	escaped_serial_write(battery_state);
	escaped_serial_write((uint8_t) int(battery_percent));
	serial_write(FEND);
#endif
}

void kiss_indicate_btpin() {
#if HAS_BLUETOOTH || HAS_BLE == true
	serial_write(FEND);
	serial_write(CMD_BT_PIN);
	escaped_serial_write(bt_ssp_pin >> 24);
	escaped_serial_write(bt_ssp_pin >> 16);
	escaped_serial_write(bt_ssp_pin >> 8);
	escaped_serial_write(bt_ssp_pin);
	serial_write(FEND);
#endif
}

void kiss_indicate_random(uint8_t byte) {
	serial_write(FEND);
	serial_write(CMD_RANDOM);
	serial_write(byte);
	serial_write(FEND);
}

void kiss_indicate_fbstate() {
	serial_write(FEND);
	serial_write(CMD_FB_EXT);
#if HAS_DISPLAY
	if (disp_ext_fb) {
		serial_write(0x01);
	} else {
		serial_write(0x00);
	}
#else
	serial_write(0xFF);
#endif
	serial_write(FEND);
}

#if MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
void kiss_indicate_device_hash() {
	serial_write(FEND);
	serial_write(CMD_DEV_HASH);
	for (int i = 0; i < DEV_HASH_LEN; i++) {
		uint8_t byte = dev_hash[i];
		escaped_serial_write(byte);
	}
	serial_write(FEND);
}

void kiss_indicate_target_fw_hash() {
	serial_write(FEND);
	serial_write(CMD_HASHES);
	serial_write(0x01);
	for (int i = 0; i < DEV_HASH_LEN; i++) {
		uint8_t byte = dev_firmware_hash_target[i];
		escaped_serial_write(byte);
	}
	serial_write(FEND);
}

void kiss_indicate_fw_hash() {
	serial_write(FEND);
	serial_write(CMD_HASHES);
	serial_write(0x02);
	for (int i = 0; i < DEV_HASH_LEN; i++) {
		uint8_t byte = dev_firmware_hash[i];
		escaped_serial_write(byte);
	}
	serial_write(FEND);
}

void kiss_indicate_bootloader_hash() {
	serial_write(FEND);
	serial_write(CMD_HASHES);
	serial_write(0x03);
	for (int i = 0; i < DEV_HASH_LEN; i++) {
		uint8_t byte = dev_bootloader_hash[i];
		escaped_serial_write(byte);
	}
	serial_write(FEND);
}

void kiss_indicate_partition_table_hash() {
	serial_write(FEND);
	serial_write(CMD_HASHES);
	serial_write(0x04);
	for (int i = 0; i < DEV_HASH_LEN; i++) {
		uint8_t byte = dev_partition_table_hash[i];
		escaped_serial_write(byte);
	}
	serial_write(FEND);
}
#endif

void kiss_indicate_fb() {
	serial_write(FEND);
	serial_write(CMD_FB_READ);
#if HAS_DISPLAY
	for (int i = 0; i < 512; i++) {
		uint8_t byte = fb[i];
		escaped_serial_write(byte);
	}
#else
	serial_write(0xFF);
#endif
	serial_write(FEND);
}

void kiss_indicate_disp() {
	serial_write(FEND);
	serial_write(CMD_DISP_READ);
#if HAS_DISPLAY
	uint8_t *da = disp_area.getBuffer();
	uint8_t *sa = stat_area.getBuffer();
	for (int i = 0; i < 512; i++) { escaped_serial_write(da[i]); }
	for (int i = 0; i < 512; i++) { escaped_serial_write(sa[i]); }
#else
	serial_write(0xFF);
#endif
	serial_write(FEND);
}

void kiss_indicate_ready() {
	serial_write(FEND);
	serial_write(CMD_READY);
	serial_write(0x01);
	serial_write(FEND);
}

void kiss_indicate_not_ready() {
	serial_write(FEND);
	serial_write(CMD_READY);
	serial_write(0x00);
	serial_write(FEND);
}

void kiss_indicate_promisc() {
	serial_write(FEND);
	serial_write(CMD_PROMISC);
	if (promisc) {
		serial_write(0x01);
	} else {
		serial_write(0x00);
	}
	serial_write(FEND);
}

void kiss_indicate_detect() {
	serial_write(FEND);
	serial_write(CMD_DETECT);
	serial_write(DETECT_RESP);
	serial_write(FEND);
}

void kiss_indicate_version() {
	serial_write(FEND);
	serial_write(CMD_FW_VERSION);
	serial_write(MAJ_VERS);
	serial_write(MIN_VERS);
	serial_write(FEND);
}

void kiss_indicate_platform() {
	serial_write(FEND);
	serial_write(CMD_PLATFORM);
	serial_write(PLATFORM);
	serial_write(FEND);
}

void kiss_indicate_board() {
	serial_write(FEND);
	serial_write(CMD_BOARD);
	serial_write(BOARD_MODEL);
	serial_write(FEND);
}

void kiss_indicate_mcu() {
	serial_write(FEND);
	serial_write(CMD_MCU);
	serial_write(MCU_VARIANT);
	serial_write(FEND);
}

inline bool isSplitPacket(uint8_t header) {
	return (header & FLAG_SPLIT);
}

inline uint8_t packetSequence(uint8_t header) {
	return header >> 4;
}

void setPreamble() {
	if (radio_online) LoRa->setPreambleLength(lora_preamble_symbols);
	kiss_indicate_phy_stats();
}

void updateBitrate() {
#if MCU_VARIANT == MCU_ESP32 || MCU_VARIANT == MCU_NRF52
	if (!radio_online) {
		lora_bitrate = 0;
	} else {
		lora_symbol_rate = (float)lora_bw / (float)(pow(2, lora_sf));
		lora_symbol_time_ms = (1.0 / lora_symbol_rate) * 1000.0;
		lora_bitrate = (uint32_t)(lora_sf * ((4.0 / (float)lora_cr) / ((float)(pow(2, lora_sf)) / ((float)lora_bw / 1000.0))) * 1000.0);
		lora_us_per_byte = 1000000.0 / ((float)lora_bitrate / 8.0);

		bool fast_rate = lora_bitrate > LORA_FAST_THRESHOLD_BPS;
		lora_limit_rate = lora_bitrate > LORA_LIMIT_THRESHOLD_BPS;

		int csma_slot_min_ms = CSMA_SLOT_MIN_MS;
		float lora_preamble_target_ms = LORA_PREAMBLE_TARGET_MS;
		if (fast_rate) {
			csma_slot_min_ms -= CSMA_SLOT_MIN_FAST_DELTA;
			lora_preamble_target_ms -= LORA_PREAMBLE_FAST_DELTA;
		}

		csma_slot_ms = lora_symbol_time_ms * CSMA_SLOT_SYMBOLS;
		if (csma_slot_ms > CSMA_SLOT_MAX_MS) { csma_slot_ms = CSMA_SLOT_MAX_MS; }
		if (csma_slot_ms < CSMA_SLOT_MIN_MS) { csma_slot_ms = csma_slot_min_ms; }
		difs_ms = CSMA_SIFS_MS + 2 * csma_slot_ms;

		float target_preamble_symbols = lora_preamble_target_ms / lora_symbol_time_ms;
		if (target_preamble_symbols < LORA_PREAMBLE_SYMBOLS_MIN) {
			target_preamble_symbols = LORA_PREAMBLE_SYMBOLS_MIN;
		} else {
			target_preamble_symbols = (ceil)(target_preamble_symbols);
		}

		lora_preamble_symbols = (long)target_preamble_symbols;
		setPreamble();
		lora_preamble_time_ms = (ceil)(lora_preamble_symbols * lora_symbol_time_ms);
		lora_header_time_ms = (ceil)(PHY_HEADER_LORA_SYMBOLS * lora_symbol_time_ms);
	}
#endif
}

void setSpreadingFactor() {
	if (radio_online) LoRa->setSpreadingFactor(lora_sf);
	updateBitrate();
}

void setCodingRate() {
	if (radio_online) LoRa->setCodingRate4(lora_cr);
	updateBitrate();
}

void set_implicit_length(uint8_t len) {
	implicit_l = len;
	if (implicit_l != 0) {
		implicit = true;
	} else {
		implicit = false;
	}
}

int getTxPower() {
	uint8_t txp = LoRa->getTxPower();
	return (int)txp;
}

void setTXPower() {
	if (radio_online) {
		if (model == MODEL_11) LoRa->setTxPower(lora_txp, PA_OUTPUT_RFO_PIN);
		if (model == MODEL_12) LoRa->setTxPower(lora_txp, PA_OUTPUT_RFO_PIN);

		if (model == MODEL_C6) LoRa->setTxPower(lora_txp, PA_OUTPUT_RFO_PIN);
		if (model == MODEL_C7) LoRa->setTxPower(lora_txp, PA_OUTPUT_RFO_PIN);

		if (model == MODEL_A1) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_A2) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_A3) LoRa->setTxPower(lora_txp, PA_OUTPUT_RFO_PIN);
		if (model == MODEL_A4) LoRa->setTxPower(lora_txp, PA_OUTPUT_RFO_PIN);
		if (model == MODEL_A5) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_A6) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_A7) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_A8) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_A9) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_AA) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_AC) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);

		if (model == MODEL_BA) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_BB) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_B3) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_B4) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_B8) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_B9) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);

		if (model == MODEL_C4) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_C9) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_C5) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_CA) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);

		if (model == MODEL_D4) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_D9) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);

		if (model == MODEL_DB) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_DC) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);

		if (model == MODEL_DD) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_DE) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);

		if (model == MODEL_E4) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_E9) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_E3) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_E8) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);

		if (model == MODEL_FE) LoRa->setTxPower(lora_txp, PA_OUTPUT_PA_BOOST_PIN);
		if (model == MODEL_FF) LoRa->setTxPower(lora_txp, PA_OUTPUT_RFO_PIN);
	}
}


void getBandwidth() {
	if (radio_online) {
		lora_bw = LoRa->getSignalBandwidth();
	}
	updateBitrate();
}

void setBandwidth() {
	if (radio_online) {
		LoRa->setSignalBandwidth(lora_bw);
		getBandwidth();
	}
}

void getFrequency() {
	if (radio_online) {
		lora_freq = LoRa->getFrequency();
	}
}

void setFrequency() {
	if (radio_online) {
		LoRa->setFrequency(lora_freq);
		getFrequency();
	}
}

uint8_t getRandom() {
	return random(0xFF);
}

void promisc_enable() {
	promisc = true;
}

void promisc_disable() {
	promisc = false;
}


void init_channel_stats() {
#if MCU_VARIANT == MCU_ESP32
	for (uint16_t ai = 0; ai < DCD_SAMPLES; ai++) { util_samples[ai] = false; }
	for (uint16_t ai = 0; ai < AIRTIME_BINS; ai++) { airtime_bins[ai] = 0; }
	for (uint16_t ai = 0; ai < AIRTIME_BINS; ai++) { longterm_bins[ai] = 0.0; }
	local_channel_util = 0.0;
	total_channel_util = 0.0;
	airtime = 0.0;
	longterm_airtime = 0.0;
#endif
}

#endif /* KISS_H */
