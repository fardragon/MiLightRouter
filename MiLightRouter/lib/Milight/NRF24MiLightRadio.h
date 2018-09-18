#ifndef _NRF24_MILIGHT_RADIO_H_
#define _NRF24_MILIGHT_RADIO_H_

#include <Arduino.h>
#include <RF24.h>
#include <PL1167_nRF24.h>

class NRF24MiLightRadio
{
  public:
    NRF24MiLightRadio(const uint8_t RF24_CE, const uint8_t RF24_CS);
    int begin();
    bool available();
    int read(uint8_t frame[], size_t &frame_length);
    int dupesReceived();
    int write(uint8_t frame[], size_t frame_length);
    void resend();
    int configure();

  private:
    RF24 _radio;
    PL1167_nRF24 _pl1167;
    uint32_t _prev_packet_id;

    uint8_t _packet[10];
    uint8_t _out_packet[10];
    bool _waiting;
    int _dupes_received;
    const uint8_t _channels[3] = {9, 40, 71};
};

#endif
