#ifndef __MILIGHT_H__
#define __MILIGHT_H__

#include <Arduino.h>
#include "Remote.h"


class Milight
{
    public:
    Milight(const int radio_ce, const int radio_cs);


    void TestPair();

    private:

    void SendCommand(uint8_t command[7]);

    private:
    Remote m_remote;
    uint8_t m_packetID;

};





#endif