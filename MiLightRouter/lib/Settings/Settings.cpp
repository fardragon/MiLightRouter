#include "Settings.h"
#include <EEPROM.h>

std::string Settings::ReadServerAddress()
{
    EEPROM.begin(MQTT_SERVER_ADDRESS + MQTT_SERVER_SIZE);

    std::string result;
    for (uint8_t i = 0; i < MQTT_SERVER_SIZE; ++i)
    {
        auto byte = EEPROM.read(MQTT_SERVER_ADDRESS + i);
        if (byte == '\0')
            break;
        result += byte;
    }

    EEPROM.end();
    return result;
}

void Settings::WriteServerAddress(const std::string &address)
{
    EEPROM.begin(MQTT_SERVER_ADDRESS + MQTT_SERVER_SIZE);
    for (uint8_t i = 0; i < MQTT_SERVER_SIZE; ++i)
    {
        if (i < address.length())
        {
            EEPROM.write(MQTT_SERVER_ADDRESS + i, address[i]);
        }
        else
        {
            EEPROM.write(MQTT_SERVER_ADDRESS + i, '\0');
        }
    }
    EEPROM.end();
}

uint16_t Settings::ReadServerPort()
{
    EEPROM.begin(MQTT_PORT_ADDRESS + MQTT_PORT_SIZE);
    uint16_t result;
    EEPROM.get(MQTT_PORT_ADDRESS, result);
    EEPROM.end();
    return result;
}

void Settings::WriteServerPort(uint16_t port)
{
    EEPROM.begin(MQTT_PORT_ADDRESS + MQTT_PORT_SIZE);
    EEPROM.put(MQTT_PORT_ADDRESS, port);
    EEPROM.end();
}

bool Settings::ReadUseCredentials()
{
    EEPROM.begin(MQTT_USE_CREDENTIALS_ADDRESS + MQTT_USE_CREDENTIALS_SIZE);
    bool result = false;
    EEPROM.get(MQTT_USE_CREDENTIALS_ADDRESS, result);
    EEPROM.end();
    return result;
}

void Settings::WriteUseCredentials(bool use)
{
    EEPROM.begin(MQTT_USE_CREDENTIALS_ADDRESS + MQTT_USE_CREDENTIALS_SIZE);
    EEPROM.put(MQTT_USE_CREDENTIALS_ADDRESS, use);
    EEPROM.end();
}

std::string Settings::ReadUsername()
{
    EEPROM.begin(MQTT_USERNAME_ADDRESS + MQTT_USERNAME_SIZE);
    std::string result;
    for (uint8_t i = 0; i < MQTT_USERNAME_SIZE; ++i)
    {
        auto byte = EEPROM.read(MQTT_USERNAME_ADDRESS + i);
        if (byte == '\0')
            break;
        result += byte;
    }
    EEPROM.end();
    return result;
}

void Settings::WriteUsername(const std::string &username)
{
    EEPROM.begin(MQTT_USERNAME_ADDRESS + MQTT_USERNAME_SIZE);
    for (uint8_t i = 0; i < MQTT_USERNAME_SIZE; ++i)
    {
        if (i < username.length())
        {
            EEPROM.write(MQTT_USERNAME_ADDRESS + i, username[i]);
        }
        else
        {
            EEPROM.write(MQTT_USERNAME_ADDRESS + i, '\0');
        }
    }
    EEPROM.end();
}

std::string Settings::ReadPassword()
{
    EEPROM.begin(MQTT_PASSWORD_ADDRESS + MQTT_PASSWORD_SIZE);
    std::string result;
    for (uint8_t i = 0; i < MQTT_PASSWORD_SIZE; ++i)
    {
        auto byte = EEPROM.read(MQTT_PASSWORD_ADDRESS + i);
        if (byte == '\0')
            break;
        result += byte;
    }
    EEPROM.end();
    return result;
}

void Settings::WritePassword(const std::string &password)
{
    EEPROM.begin(MQTT_PASSWORD_ADDRESS + MQTT_PASSWORD_SIZE);
    for (uint8_t i = 0; i < MQTT_PASSWORD_SIZE; ++i)
    {
        if (i < password.length())
        {
            EEPROM.write(MQTT_PASSWORD_ADDRESS + i, password[i]);
        }
        else
        {
            EEPROM.write(MQTT_PASSWORD_ADDRESS + i, '\0');
        }
    }
    EEPROM.end();
}

std::string Settings::ReadMQTTTopic()
{
    EEPROM.begin(MQTT_TOPIC_ADDRESS + MQTT_TOPIC_SIZE);
    std::string result;
    for (uint8_t i = 0; i < MQTT_TOPIC_SIZE; ++i)
    {
        auto byte = EEPROM.read(MQTT_TOPIC_ADDRESS + i);
        if (byte == '\0')
            break;
        result += byte;
    }
    EEPROM.end();
    return result;
}

void Settings::WriteMQTTTopic(const std::string &topic)
{
    EEPROM.begin(MQTT_TOPIC_ADDRESS + MQTT_TOPIC_SIZE);
    for (uint8_t i = 0; i < MQTT_TOPIC_SIZE; ++i)
    {
        if (i < topic.length())
        {
            EEPROM.write(MQTT_TOPIC_ADDRESS + i, topic[i]);
        }
        else
        {
            EEPROM.write(MQTT_TOPIC_ADDRESS + i, '\0');
        }
    }
    EEPROM.end();
}


uint16_t Settings::ReadMilightDeviceID()
{
    EEPROM.begin(MILIGHT_DEVICE_ID_ADDRESS + MILIGHT_DEVICE_ID_SIZE);
    uint16_t result = EEPROM.get(MILIGHT_DEVICE_ID_ADDRESS, result);
    EEPROM.end();
    return result;

}
uint16_t Settings::ReadMilightDetectedID()
{
    EEPROM.begin(MILIGHT_DETECTED_ID_ADDRESS + MILIGHT_DETECTED_ID_SIZE);
    uint16_t result = EEPROM.get(MILIGHT_DETECTED_ID_ADDRESS, result);
    EEPROM.end();
    return result;
}
void Settings::WriteMilightDeviceID(const uint16_t id)
{
    EEPROM.begin(MILIGHT_DEVICE_ID_ADDRESS + MILIGHT_DEVICE_ID_SIZE);
    EEPROM.put(MILIGHT_DEVICE_ID_ADDRESS, id);
    EEPROM.end();
}
void Settings::WriteMilightDetectedID(const uint16_t id)
{
    EEPROM.begin(MILIGHT_DETECTED_ID_ADDRESS + MILIGHT_DETECTED_ID_SIZE);
    EEPROM.put(MILIGHT_DETECTED_ID_ADDRESS, id);
    EEPROM.end();
}

Settings eeprom_settings;