#include "Utility.h"


uint8_t* MQTT::UTF8Encode(uint8_t *address, const char *string, uint8_t length)
{
    *(address++) = 0;
    *(address++) = length;
    for (uint8_t i = 0; i < length; ++i)
    {
        *(address++) = string[i];
    }
    return address;
}


