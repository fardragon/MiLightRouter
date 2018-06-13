#ifndef __REMOTE_H__
#define __REMOTE_H__

#include "Radio.h"

class Remote
{
public:
    Remote(const int radio_ce, const int radio_cs);
    bool Send(uint8_t *data, size_t size);

private:
    void InternalSend();

private:
    Radio m_radio;
    bool m_error;
    uint8_t m_outPacket[8];

};







#endif