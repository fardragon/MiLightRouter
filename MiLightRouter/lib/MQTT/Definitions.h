#ifndef __MQTTDEFS__
#define __MQTTDEFS__


#include <Arduino.h>
#include <functional>
#include <string>
#include "Utility.h"



//Control packet types
#define MQTT_PACKET_CONNECT   0b00010000
#define MQTT_PACKET_CONNACK   0b00100000
#define MQTT_PACKET_PINGREQ   0b11000000
#define MQTT_PACKET_PINGRESP  0b11010000
#define MQTT_PACKET_SUBSCRIBE 0b10000010
#define MQTT_PACKET_SUBACK    0b10010000
#define MQTT_PACKET_PUBLISH   0b00110000
#define MQTT_PACKET_PUBACK    0b01000000

//Connect flags
#define MQTT_CF_CLEAN_SESSION _BV(1)
#define MQTT_CF_WILL_FLAG _BV(2)
#define MQTT_CF_WILL_QOS_L _BV(3)
#define MQTT_CF_WILL_QOS_H _BV(4)
#define MQTT_CF_WILL_RETAIN _BV(5)
#define MQTT_CF_PASSWORD _BV(6)
#define MQTT_CF_USERNAME _BV(7)

namespace MQTT
{
    enum class Status
    {
        Disconnected,
        Connecting,
        Connected
    };

    struct Subscription
    {
        Subscription(std::string Topic, std::function<void(uint8_t*,uint8_t)> Handler, uint16_t ID)
            : m_Topic(Topic), m_Handler(Handler), m_Ack(false),  m_ID(ID)
            {
            };
        std::string m_Topic;
        std::function<void(uint8_t*,uint8_t)> m_Handler;
        bool m_Ack;
        uint16_t m_ID;
    };




}
#endif