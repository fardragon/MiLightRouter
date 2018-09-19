#include "Milight.h"

#include <Settings.h>

Milight::Milight(const uint8_t RF24_CE, const uint8_t RF24_CS)
    : m_radio(RF24_CE, RF24_CS), m_packet_id(0)
{
    auto device_id = eeprom_settings.ReadMilightDeviceID();
    m_device_id[0] = device_id >> 8;
    m_device_id[1] = device_id & 0x00FF;
}

uint8_t Milight::init()
{
    return m_radio.begin();
}

void Milight::send_command(const MilightCommand command, uint8_t color, uint8_t brightess)
{

    switch (command)
    {
    case MilightCommand::ALL_ON:
    case MilightCommand::ALL_OFF:
    case MilightCommand::GROUP_1_ON:
    case MilightCommand::GROUP_1_OFF:
    case MilightCommand::GROUP_2_ON:
    case MilightCommand::GROUP_2_OFF:
    case MilightCommand::GROUP_3_ON:
    case MilightCommand::GROUP_3_OFF:
    case MilightCommand::GROUP_4_ON:
    case MilightCommand::GROUP_4_OFF:
    case MilightCommand::SPEED_PLUS:
    case MilightCommand::SPEED_MINUS:
    case MilightCommand::MODES:
    case MilightCommand::BRIGHTNESS:
    case MilightCommand::COLOR:
    {
        uint8_t data[7] = {0xB0, m_device_id[0], m_device_id[1], color, brightess, static_cast<uint8_t>(command), m_packet_id++};
        _send_command(data, 7);
        break;
    }

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
    case MilightCommand::LAST:
    default:
    {
        Serial.println("Invalid command executed");
        break;
    }
    }
}

void Milight::_send_command(uint8_t data[], uint8_t data_size)
{
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
        Serial.printf("Color: %u ", buffer[3]);
        Serial.printf("Brightness: %u ", buffer[4]);
        uint16_t device_id = (buffer[1] << 8) | buffer[2];
        Serial.printf("Device id: %u\n\r", device_id);
        eeprom_settings.WriteMilightDetectedID(device_id);
        Serial.println();
    }
}

MilightCommand string_to_command(const std::string str)
{
    if (str == "all-on") return MilightCommand::ALL_ON;
    if (str == "all-off") return MilightCommand::ALL_OFF;
    if (str == "group1-on") return MilightCommand::GROUP_1_ON;
    if (str == "group1-off") return MilightCommand::GROUP_1_OFF;
    if (str == "group2-on") return MilightCommand::GROUP_2_ON;
    if (str == "group2-off") return MilightCommand::GROUP_2_OFF;
    if (str == "group3-on") return MilightCommand::GROUP_3_ON;
    if (str == "group3-off") return MilightCommand::GROUP_3_OFF;
    if (str == "group4-on") return MilightCommand::GROUP_4_ON;
    if (str == "group4-off") return MilightCommand::GROUP_4_OFF;
    if (str == "speed-plus") return MilightCommand::SPEED_PLUS;
    if (str == "speed-minus") return MilightCommand::SPEED_MINUS;
    if (str == "modes") return MilightCommand::MODES;
    if (str == "brightness") return MilightCommand::BRIGHTNESS;
    if (str == "color") return MilightCommand::COLOR;
    if (str == "all-white") return MilightCommand::ALL_WHITE;
    if (str == "group1-unpair-white") return MilightCommand::GROUP_1_UNPAIR_WHITE;
    if (str == "group2-unpair-white") return MilightCommand::GROUP_2_UNPAIR_WHITE;
    if (str == "group3-unpair-white") return MilightCommand::GROUP_3_UNPAIR_WHITE;
    if (str == "group4-unpair-white") return MilightCommand::GROUP_4_UNPAIR_WHITE;
    if (str == "group1-pair") return MilightCommand::GROUP_1_PAIR;
    if (str == "group2-pair") return MilightCommand::GROUP_2_PAIR;
    if (str == "group3-pair") return MilightCommand::GROUP_3_PAIR;
    if (str == "group4-pair") return MilightCommand::GROUP_4_PAIR;
    return MilightCommand::LAST;
}

uint8_t level_to_brightness(const uint8_t level)
{
    if (level == 0) return 128;
    if (level == 1) return 120;
    if (level == 2) return 112;
    if (level == 3) return 104;
    if (level == 4) return 96;
    if (level == 5) return 88;
    if (level == 6) return 80;
    if (level == 7) return 72;
    if (level == 8) return 64;
    if (level == 9) return 56;
    if (level == 10) return 48;
    if (level == 11) return 40;
    if (level == 12) return 32;
    if (level == 13) return 24;
    if (level == 14) return 16;
    if (level == 15) return 8;
    if (level == 16) return 0;
    if (level == 17) return 248;
    if (level == 18) return 240;
    if (level == 19) return 232;
    if (level == 20) return 224;
    if (level == 21) return 216;
    if (level == 22) return 208;
    if (level == 23) return 200;
    if (level == 24) return 192;
    return 184;
}