#include "Radio.h"
Radio::Radio(const int radio_ce, const int radio_cs)
    : m_radio(radio_ce, radio_cs)
{
    if (m_radio.begin() && this->CalculateParameters())
    {
        Serial.println("Radio init ok");
    }
    else
    {
        Serial.println("Radio init error");
    }

}


static const uint8_t pipe[] = {0xd1, 0x28, 0x5e, 0x55, 0x55};

bool Radio::CalculateParameters()
{
    int nrf_addressLength = (m_preambleLength - 1) + m_syncwordLength;

    int nrf_addressOvf = 0;

    if (nrf_addressLength > 5)
    {
        nrf_addressLength = nrf_addressLength - 5;
        nrf_addressLength = 5;
    }

    int nrf_packetLength = nrf_addressOvf + ( (m_trailerLength + 7) / 8) + m_maxPacketLength;
    if (m_crc) 
    {
        nrf_packetLength += 2;
    }

    if (nrf_packetLength > sizeof(m_packet) || nrf_addressLength < 3) return false;

    uint8_t preamble = 0;

    if (m_syncword0 & 0x01) 
    {
        preamble = 0x55;
    } 
    else 
    {
        preamble = 0xAA;
    }


    int nrf_addressPos = nrf_addressLength;
    for (int i = 0; i < m_preambleLength - 1; i++) 
    {
        m_nrf_pipe[ --nrf_addressPos ] = Radio::ReverseBits(preamble);
    }


    if (nrf_addressPos) 
    {
        m_nrf_pipe[ --nrf_addressPos ] = Radio::ReverseBits(m_syncword0 & 0xff);
    }
    
    if (nrf_addressPos) 
    {
        m_nrf_pipe[ --nrf_addressPos ] = Radio::ReverseBits( (m_syncword0 >> 8) & 0xff);
    }
    
    if (m_syncwordLength == 4) 
    {
        if (nrf_addressPos) 
        {
            m_nrf_pipe[ --nrf_addressPos ] = Radio::ReverseBits(m_syncword3 & 0xff);
        }
        if (nrf_addressPos) 
        {
            m_nrf_pipe[ --nrf_addressPos ] = Radio::ReverseBits( (m_syncword3 >> 8) & 0xff);
        }
    }

    m_receive_length = nrf_packetLength;
    m_preamble = preamble;

    m_nrf_pipe_length = nrf_addressLength;
    m_radio.setAddressWidth(m_nrf_pipe_length);
    m_radio.openWritingPipe(m_nrf_pipe);
    //m_radio.openReadingPipe(1, m_nrf_pipe);

    m_radio.setChannel(2 + m_channel);


    m_radio.setPayloadSize( m_packet_length );
    m_radio.setAutoAck(false);
    m_radio.setPALevel(RF24_PA_MAX);
    m_radio.setDataRate(RF24_1MBPS);
    m_radio.disableCRC();

    return true;    
}

uint8_t Radio::ReverseBits(uint8_t data)
{
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) 
    {
        result <<= 1;
        result |= data & 1;
        data >>= 1;
    }
    return result;
}

bool Radio::SetPreambleLength(const uint8_t length)
{
    if (length > 8)
    {
        return false;
    }
    m_preambleLength = length;
    return this->CalculateParameters();
}

bool Radio::SetSyncword(const uint16_t syncword0, const uint16_t syncword3)
{
    m_syncwordLength = 4;
    m_syncword0 = syncword0;
    m_syncword3 = syncword3;
    return this->CalculateParameters();
}

bool Radio::SetMaxPacketLength(const uint8_t maxPacketLength)
{
    m_maxPacketLength = maxPacketLength;
    return this->CalculateParameters();
}

bool Radio::SetTrailerLength(const uint8_t trailerLength)
{
    if (trailerLength < 4) return false;
    if (trailerLength > 18) return false;
    if (trailerLength & 0x01) return false;
    m_trailerLength = trailerLength;
    return this->CalculateParameters();
}

bool Radio::SetCRC(const bool crc)
{
    m_crc = crc;
    return this->CalculateParameters();
}

size_t Radio::WriteData(const uint8_t *data, size_t length)
{
    if (length > sizeof(m_packet)) length = sizeof(m_packet);
    memcpy(m_packet, data, length);
    m_packet_length = length;
    m_received = false;
    return length;
}

bool Radio::TransmitData(const uint8_t channel)
{
    if (channel != m_channel) 
    {
        m_channel = channel;
        if (!this->CalculateParameters()) return false;
    }

    m_radio.stopListening();
    uint8_t tmp[sizeof(m_packet)];

    uint8_t trailer = (m_packet[0] & 1) ? 0x55 : 0xAA;  // NOTE: ???????
    int outp = 0;

    for (; outp < m_receive_length; outp++)
    {
        uint8_t outbyte = 0;

        if (outp + 1 + m_nrf_pipe_length < m_preambleLength) 
        {
            outbyte = m_preamble;
        }
        else if (outp + 1 + m_nrf_pipe_length < m_preambleLength + m_syncwordLength) 
        {
            int syncp = outp - m_preambleLength + 1 + m_nrf_pipe_length;
            switch (syncp) 
            {
                case 0:
                outbyte = m_syncword0 & 0xFF;
                break;
                case 1:
                outbyte = (m_syncword0 >> 8) & 0xFF;
                break;
                case 2:
                outbyte = m_syncword3 & 0xFF;
                break;
                case 3:
                outbyte = (m_syncword3 >> 8) & 0xFF;
                break;
            }
        }
        else if (outp + 1 + m_nrf_pipe_length < m_preambleLength + m_syncwordLength + (m_trailerLength / 8) ) 
        {
            outbyte = trailer;
        } 
        else 
        {
            break;
        }
        tmp[outp] = Radio::ReverseBits(outbyte);
    }

    int buffer_fill;
    bool last_round = false;
    uint16_t buffer = 0;
    uint16_t crc;
    if (m_crc) 
    {
        crc = Radio::CalcCRC(m_packet, m_packet_length);
    }

    buffer = trailer >> (8 - (m_trailerLength % 8));
    buffer_fill = m_trailerLength % 8;
    for (int inp = 0; inp < m_packet_length + (m_crc ? 2 : 0) + 1; inp++) 
    {
        if (inp < m_packet_length) 
        {
            buffer |= m_packet[inp] << buffer_fill;
            buffer_fill += 8;
        }
        else if (m_crc && inp < m_packet_length + 2) 
        {
            buffer |= ((crc >>  ( (inp - m_packet_length) * 8)) & 0xff) << buffer_fill;
            buffer_fill += 8;
        }
        else
        {
            last_round = true;
        }

        while (buffer_fill > (last_round ? 0 : 8)) 
        {
            if (outp >= sizeof(tmp)) return false;
            tmp[outp++] = Radio::ReverseBits(buffer & 0xff);
            buffer >>= 8;
            buffer_fill -= 8;
        }
    }

    m_radio.write(tmp, outp);
    return 0;
}

#define CRC_POLY 0x8408

uint16_t Radio::CalcCRC(uint8_t *data, size_t length)
{
    uint16_t state = 0;
    for (size_t i = 0; i < length; i++) 
    {
        uint8_t byte = data[i];
        for (int j = 0; j < 8; j++) 
        {
            if ((byte ^ state) & 0x01) 
            {
            state = (state >> 1) ^ CRC_POLY;
            }
            else 
            {
                state = state >> 1;
            }

            byte = byte >> 1;
        }
    }
    return state;
}