#ifndef __MILIGHT_H__
#define __MILIGHT_H__

#include <Arduino.h>
#include "MiLightRadio.h"
#include <RF24.h>
#include "PL1167_nRF24.h"


class Milight
{
    public:
    Milight(const int radio_ce, const int radio_cs);

    bool Avalaible();
    void Receive();
    
    void TestOn();
    void TestOff();

    private:

    void SendCommand(uint8_t command[7]);


    private:
    RF24 *m_radio;
    PL1167_nRF24 *m_abstract;
    MiLightRadio *m_remote;
    uint8_t m_packetID;

};





#endif