#include "Milight.h"

Milight::Milight(const int radio_ce, const int radio_cs)
    : m_remote(radio_ce, radio_cs), m_packetID(0x50)
{

}

void Milight::TestPair()
{
    Serial.println("Test pair command");
    uint8_t packet[7] = {0xb0, 0xf2, 0xea, 0x00, 0XD1, 0x03, m_packetID++};

    this->SendCommand(packet);

    packet[5] = 0x13;
    packet[6] = m_packetID++;

    this->SendCommand(packet);

    delay(300);

    this->SendCommand(packet);

}

void Milight::SendCommand(uint8_t command[7])
{
    m_remote.Send(command, sizeof(command));
    delay(50);

    for (uint8_t i = 0; i < 50; ++i)
    {
        m_remote.Resend();
        delay(1);
    }
}