#ifndef __MQTT__
#define __MQTT__

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "Definitions.h"
#define MQTT_MAX_PACKET_SIZE 255
#define MQTT_TIMEOUT 500
#define MQTT_KEEP_ALIVE 300

namespace MQTT
{
    class MQTTClient
    {
        public:
        MQTTClient();
        MQTT::Status Initialize(IPAddress ServerAddress, uint16_t Port);
        MQTT::Status Loop();

        private:
        bool ReceivePacket();
        bool InterpretPacket();

        bool ReadByte(uint8_t *dest, uint16_t *offset);
        bool ReadByte(uint8_t *dest);
        void SendData();

        uint8_t LengthBytes();

        bool Packet_CONNACK();
        bool Packet_PINGRESP();

        void GenerateConnectPacket();
        void GeneratePingPacket();


        private:
        MQTT::Status m_Status;
        WiFiClient m_Client;

        uint8_t m_Buffer[MQTT_MAX_PACKET_SIZE];
        uint8_t m_BufferLength;

        uint32_t m_LastActivity;
        bool m_SentPing;

    };
}








#endif