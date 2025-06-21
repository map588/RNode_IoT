
#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

void serial_write(uint8_t byte) {
#if HAS_BLUETOOTH || HAS_BLE == true
  if (bt_state != BT_STATE_CONNECTED) {
    Serial.write(byte);
  } else {
    SerialBT.write(byte);
#if MCU_VARIANT == MCU_NRF52 && HAS_BLE
    // This ensures that the TX buffer is flushed after a frame is queued in serial.
    // serial_in_frame is used to ensure that the flush only happens at the end of the frame
    if (serial_in_frame && byte == FEND) {
      SerialBT.flushTXD();
      serial_in_frame = false;
    } else if (!serial_in_frame && byte == FEND) {
      serial_in_frame = true;
    }
#endif
  }
#else
  Serial.write(byte);
#endif
}

void escaped_serial_write(uint8_t byte) {
  if (byte == FEND) {
    serial_write(FESC);
    byte = TFEND;
  }
  if (byte == FESC) {
    serial_write(FESC);
    byte = TFESC;
  }
  serial_write(byte);
}

#endif  // SERIAL_COMM_H