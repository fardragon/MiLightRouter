#ifndef __MQTTUTILS__
#define __MQTTUTILS__

#include <Arduino.h>
#include <string>

namespace MQTT
{
    uint8_t* UTF8Encode(uint8_t *address, const char *string, uint8_t length);
    uint8_t* UTF8Decode(uint8_t *address, std::string &result);




}
#endif