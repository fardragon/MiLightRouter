#include "Milight.h"

Milight::Milight(const uint8_t RF24_CE, const uint8_t RF24_CS, uint16_t device_id)
    : m_radio(RF24_CE, RF24_CS), m_packet_id(0)
{
    m_device_id[0] = device_id >> 8;
    m_device_id[1] = device_id & 0x00FF;
}

uint8_t Milight::init()
{
    return m_radio.begin();
}

void Milight::send_command(const MilightCommand command, uint8_t color, uint8_t brightess)
{
    if (command <= MilightCommand::COLOR)
    {
        uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], color, brightess, static_cast<uint8_t>(command), m_packet_id++};
        _send_command(data, 7);
    }
    else
    {
        switch (command)
        {
        case MilightCommand::ALL_WHITE:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], color, brightess, 0x01, m_packet_id++};
            _send_command(data, 7);
            data[5] = 0x11;
            data[6] = m_packet_id++;
            _send_command(data, 7);
            break;
        }
        case MilightCommand::GROUP_1_UNPAIR_WHITE:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], color, brightess, 0x03, m_packet_id++};
            _send_command(data, 7);
            data[5] = 0x13;
            data[6] = m_packet_id++;
            _send_command(data, 7);
            break;
        }
        case MilightCommand::GROUP_2_UNPAIR_WHITE:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], color, brightess, 0x05, m_packet_id++};
            _send_command(data, 7);
            data[5] = 0x15;
            data[6] = m_packet_id++;
            _send_command(data, 7);
            break;
        }
        case MilightCommand::GROUP_3_UNPAIR_WHITE:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], color, brightess, 0x07, m_packet_id++};
            _send_command(data, 7);
            data[5] = 0x17;
            data[6] = m_packet_id++;
            _send_command(data, 7);
            break;
        }
        case MilightCommand::GROUP_4_UNPAIR_WHITE:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], color, brightess, 0x09, m_packet_id++};
            _send_command(data, 7);
            data[5] = 0x19;
            data[6] = m_packet_id++;
            _send_command(data, 7);
            break;
        }
        case MilightCommand::GROUP_1_PAIR:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], 0x1, 0x1, 0x03, m_packet_id++};
            _send_command(data, 7);

            for (uint8_t i = 0; i < 4; ++i)
            {
                data[6] = m_packet_id++;
                _send_command(data, 7);
                delay(100);
            }
            break;
        }
        case MilightCommand::GROUP_2_PAIR:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], 0x2, 0x2, 0x05, m_packet_id++};
            _send_command(data, 7);

            for (uint8_t i = 0; i < 4; ++i)
            {
                data[6] = m_packet_id++;
                _send_command(data, 7);
                delay(100);
            }
            break;
        }
        case MilightCommand::GROUP_3_PAIR:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], 0x3, 0x3, 0x07, m_packet_id++};
            _send_command(data, 7);

            for (uint8_t i = 0; i < 4; ++i)
            {
                data[6] = m_packet_id++;
                _send_command(data, 7);
                delay(100);
            }
            break;
        }
        case MilightCommand::GROUP_4_PAIR:
        {
            uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], 0x4, 0x4, 0x09, m_packet_id++};
            _send_command(data, 7);

            for (uint8_t i = 0; i < 4; ++i)
            {
                data[6] = m_packet_id++;
                _send_command(data, 7);
                delay(100);
            }
            break;
        }
        }
    }
}

void Milight::_send_command(uint8_t data[], uint8_t data_size)
{
    Serial.println("send_command");
    m_radio.write(data, data_size);

    for (uint8_t i = 0; i < 50; ++i)
        m_radio.resend();
}

void Milight::receive()
{
    if (m_radio.available())
    {
        uint8_t buffer[10];
        size_t size = sizeof(buffer);
        m_radio.read(buffer, size);
        Serial.print("Data received: ");
        for (uint8_t i = 0; i < size; ++i)
        {
            Serial.print(buffer[i], 16);
            Serial.print(" ");    
        }
        Serial.printf("Brightness: %u\n\r", buffer[4]);
        Serial.println();
    }
}
