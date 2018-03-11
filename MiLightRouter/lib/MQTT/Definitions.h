#ifndef __MQTTDEFS__
#define __MQTTDEFS__


#include <Arduino.h>
#include "Utility.h"

//Control packet types
#define MQTT_PACKET_CONNECT 0b00010000

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
        Error
    };


}
#endif