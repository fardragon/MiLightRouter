#ifndef __RADIO_H__
#define __RADIO_H__

#include <RF24.h>
#include <Arduino.h>


class Radio
{
public:
    Radio(const int radio_ce, const int radio_cs);

    bool SetPreambleLength(const uint8_t length);
    bool SetSyncword(const uint16_t syncword0, const uint16_t syncword3);
    bool SetMaxPacketLength(const uint8_t maxPacketLength);
    bool SetTrailerLength(const uint8_t trailerLength);
    bool SetCRC(const bool crc);

    size_t WriteData(const uint8_t *data, size_t length);
    bool TransmitData(const uint8_t channel);

    static uint8_t ReverseBits(uint8_t data);
    static uint16_t CalcCRC(uint8_t *data, size_t length);

private:
    bool CalculateParameters();


private:
    RF24 m_radio;

    bool m_crc;
    uint8_t m_preambleLength = 1;
    uint16_t m_syncword0 = 0, m_syncword3 = 0;
    uint8_t m_syncwordLength = 4;
    uint8_t m_trailerLength = 4;
    uint8_t m_maxPacketLength = 8;

    uint8_t m_channel = 0;

    uint8_t m_nrf_pipe[5];
    uint8_t m_nrf_pipe_length;

    uint8_t m_packet_length = 0;
    uint8_t m_receive_length = 0;
    uint8_t m_preamble = 0;
    uint8_t m_packet[32];
    bool m_received = false;

};

#endif