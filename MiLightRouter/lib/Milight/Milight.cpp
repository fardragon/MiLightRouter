#include "Milight.h"

Milight::Milight(const int radio_ce, const int radio_cs)
    : m_packetID(0x50)
{
    m_radio = new RF24(radio_ce, radio_cs);
    m_abstract = new PL1167_nRF24(*m_radio);
    m_remote = new MiLightRadio(*m_abstract);
    auto ret = m_remote->begin();
    if (ret != 0) Serial.println("NRF24 error!");
}

void Milight::TestOn()
{
    Serial.println("Test on command");
    uint8_t packet[7] = {0xb0, 0x5e, 0x03, 0x37, 0xb9, 0x03, m_packetID++};

    this->SendCommand(packet);

    packet[5] = 0x13;
    packet[6] = m_packetID++;

    this->SendCommand(packet);

    delay(300);

    this->SendCommand(packet);

    delay(300);

    this->SendCommand(packet);

    delay(300);

    this->SendCommand(packet);

}

void Milight::TestOff()
{
    Serial.println("Test on command");
    uint8_t packet[7] = {0xb0, 0x5e, 0x03, 0x37, 0xb9, 0x02, m_packetID++};

    this->SendCommand(packet);

    packet[5] = 0x12;
    packet[6] = m_packetID++;

    this->SendCommand(packet);

    delay(300);

    this->SendCommand(packet);
}

void Milight::SendCommand(uint8_t command[7])
{
    Serial.print("Send: ");
    for (uint8_t i = 0; i < 7; i++) 
    {
       Serial.print(command[i], HEX);
       Serial.print(' ');
    }   
    Serial.println();

    m_remote->write(command, sizeof(command));
    delay(50);

    for (uint8_t i = 0; i < 50; ++i)
    {
        m_remote->resend();
        delay(1);
    }
}
bool Milight::Avalaible()
{
    return m_remote->available();
}

void Milight::Receive()
{
    Serial.println();
    uint8_t packet[7];
    size_t size = sizeof(packet);
    m_remote->read(packet, size);
    for (uint8_t i = 0; i < 7; i++) 
    {
       Serial.print(packet[i], HEX);
       Serial.print(' ');
    }   
}