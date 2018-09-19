#ifndef __SETTINGS__
#define __SETTINGS__

#include <Arduino.h>
#include <string>


//EEPROM settings
#define MQTT_SERVER_ADDRESS 0
#define MQTT_SERVER_SIZE 32U
#define MQTT_PORT_ADDRESS (MQTT_SERVER_ADDRESS + MQTT_SERVER_SIZE)
#define MQTT_PORT_SIZE 2U
#define MQTT_USE_CREDENTIALS_ADDRESS (MQTT_PORT_ADDRESS + MQTT_PORT_SIZE)
#define MQTT_USE_CREDENTIALS_SIZE 1U
#define MQTT_USERNAME_ADDRESS (MQTT_USE_CREDENTIALS_ADDRESS + MQTT_USE_CREDENTIALS_SIZE)
#define MQTT_USERNAME_SIZE 32U
#define MQTT_PASSWORD_ADDRESS (MQTT_USERNAME_ADDRESS + MQTT_USERNAME_SIZE)
#define MQTT_PASSWORD_SIZE 32U
#define MQTT_TOPIC_ADDRESS (MQTT_PASSWORD_ADDRESS + MQTT_PASSWORD_SIZE)
#define MQTT_TOPIC_SIZE 64U
#define MILIGHT_DEVICE_ID_ADDRESS (MQTT_TOPIC_ADDRESS + MQTT_TOPIC_SIZE)
#define MILIGHT_DEVICE_ID_SIZE 2U
#define MILIGHT_DETECTED_ID_ADDRESS (MILIGHT_DEVICE_ID_ADDRESS + MILIGHT_DEVICE_ID_SIZE)
#define MILIGHT_DETECTED_ID_SIZE 2U

class Settings
{
    public:

    std::string ReadServerAddress();
    uint16_t ReadServerPort();
    bool ReadUseCredentials();
    std::string ReadUsername();
    std::string ReadPassword();
    std::string ReadMQTTTopic();
    uint16_t ReadMilightDeviceID();
    uint16_t ReadMilightDetectedID();

    void WriteServerAddress(const std::string &address);
    void WriteServerPort(uint16_t port);
    void WriteUseCredentials(bool use);
    void WriteUsername(const std::string &username);
    void WritePassword(const std::string &password);
    void WriteMQTTTopic(const std::string &topic);
    void WriteMilightDeviceID(const uint16_t id);
    void WriteMilightDetectedID(const uint16_t id);

};

extern Settings eeprom_settings;
#endif