#ifndef _NRF24_MILIGHT_RADIO_H_
#define _NRF24_MILIGHT_RADIO_H_

#include <Arduino.h>
#include <RF24.h>
#include <PL1167_nRF24.h>

class NRF24MiLightRadio
{
  public:
    NRF24MiLightRadio(const uint8_t RF24_CE, const uint8_t RF24_CS);
    uint8_t begin();
    bool available();
    int read(uint8_t frame[], size_t &frame_length);
    int write(uint8_t frame[], size_t frame_length);
    void resend();
    int configure();

  private:
    RF24 m_radio;
    PL1167_nRF24 m_pl1167;
    uint32_t m_prev_packet_id;

    uint8_t m_packet[10];
    uint8_t m_out_packet[10];
    bool m_waiting;
    int m_dupes_received;
    const uint8_t m_channels[3] = {9, 40, 71};
};

#endif
