#include "Arduino.h"
#include <RF24.h>

#ifndef PL1167_NRF24_H_
#define PL1167_NRF24_H_

class PL1167_nRF24
{
  public:
  PL1167_nRF24(RF24 &radio);
  int open();
  int setSyncword(uint16_t syncword0, uint16_t syncword3);
  int setCRC(bool crc);
  int setMaxPacketLength(uint8_t maxPacketLength);
  int writeFIFO(const uint8_t data[], size_t data_length);
  int transmit(uint8_t channel);
  int receive(uint8_t channel);
  int readFIFO(uint8_t data[], size_t &data_length);

  private:
  static uint16_t calc_crc(uint8_t *data, size_t data_length);
  static uint8_t reverse_bits(uint8_t data);

  private:
  RF24 &m_radio;

  bool m_crc;
  uint16_t m_syncword0, m_syncword3;
  uint8_t m_syncwordLength;
  uint8_t m_maxPacketLength;

  uint8_t m_channel;

  uint8_t m_nrf_pipe[5];

  uint8_t m_packet_length, m_receive_length;
  uint8_t m_packet[32];
  bool m_received;

  int recalc_parameters();
  int internal_receive();
};

#endif /* PL1167_NRF24_H_ */
