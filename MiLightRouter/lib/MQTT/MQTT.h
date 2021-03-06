#ifndef __MQTT__
#define __MQTT__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <vector>
#include <string>
#include <functional>

#include "Definitions.h"
#define MQTT_MAX_PACKET_SIZE 255
#define MQTT_TIMEOUT 500
#define MQTT_KEEP_ALIVE 300

namespace MQTT
{
    class MQTTClient
    {
        public:
        MQTTClient(std::function<void(void)> connect_callback);
        void Initialize(const char *ServerAddress, uint16_t Port);
        void Initialize(const char *ServerAddress, uint16_t Port, const std::string &username, const std::string &password);
        
        MQTT::Status Loop();

        void Subscribe(std::string topic, std::function<void(uint8_t*,uint8_t)> handler);
        
        private:

        bool ReceivePacket();
        bool InterpretPacket();

        bool ReadByte(uint8_t *dest, uint16_t *offset);
        bool ReadByte(uint8_t *dest);
        void SendData();

        uint8_t LengthBytes();
        inline uint16_t NextPacketID();

        bool Packet_CONNACK();
        bool Packet_PINGRESP();
        bool Packet_SUBACK();
        bool Packet_PUBLISH();

        void GenerateConnectPacket();
        void GenerateConnectPacket(const std::string &username, const std::string &password);
        void GeneratePingPacket();
        uint16_t GenerateSubscribePacket(const char *topic, uint8_t length);
        void GeneratePublishAckPacket(const uint16_t &packetID);




        private:
        MQTT::Status m_Status;
        WiFiClient m_Client;

        uint8_t m_Buffer[MQTT_MAX_PACKET_SIZE];
        uint8_t m_BufferLength;

        uint16_t m_PacketID;

        uint32_t m_LastActivity;
        bool m_SentPing;

        std::vector<MQTT::Subscription> m_Subscriptions;
        std::function<void(void)> m_connect_callback;

    };
}








#endif