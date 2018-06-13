#include "Remote.h"



static const uint8_t CHANNELS[] = {9, 40, 71};
#define NUM_CHANNELS (sizeof(CHANNELS)/sizeof(CHANNELS[0]))

Remote::Remote(const int radio_ce, const int radio_cs)
    :m_radio(radio_ce, radio_cs), m_error(false)
{
    if (!m_radio.SetCRC(true)) m_error = true;
    if (!m_radio.SetPreambleLength(3)) m_error = true;
    if (!m_radio.SetTrailerLength(4)) m_error = true;
    if (!m_radio.SetSyncword(0x147A, 0x258B)) m_error = true;
    if (!m_radio.SetMaxPacketLength(8)) m_error = true;
}

bool Remote::Send(uint8_t *data, size_t size)
{
    if (size > (sizeof(m_outPacket) - 1)) return false;
    memcpy(m_outPacket + 1, data, size);
    m_outPacket[0] = size;

    this->InternalSend();
    return true;
}

void Remote::Resend()
{
    this->InternalSend();
}

void Remote::InternalSend()
{
    for (uint8_t i = 0; i < NUM_CHANNELS; ++i)
    {
        m_radio.WriteData(m_outPacket, m_outPacket[0] + 1 );
        m_radio.TransmitData(CHANNELS[i]);
    }
}