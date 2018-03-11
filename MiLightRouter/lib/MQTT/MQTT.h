#ifndef __MQTT__
#define __MQTT__

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "Definitions.h"
#define MQTT_MAX_PACKET_SIZE 255

namespace MQTT
{
    class MQTTClient
    {
        public:
        MQTTClient();
        MQTT::Status Initialize(IPAddress ServerAddress, uint16_t Port);
        MQTT::Status Loop();

        private:
        void GenerateConnectPacket();

        private:
        MQTT::Status m_Status;
        WiFiClient m_Client;
        uint8_t m_Buffer[MQTT_MAX_PACKET_SIZE];
        uint8_t m_BufferLength;

    };
}








#endif