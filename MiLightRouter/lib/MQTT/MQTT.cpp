#include "MQTT.h"
#include "Utility.h"


MQTT::MQTTClient::MQTTClient()
    : m_Status(MQTT::Status::Disconnected)
{

};

MQTT::Status MQTT::MQTTClient::Initialize(IPAddress ServerAddress, uint16_t Port)
{
    if(!m_Client.connect(ServerAddress,Port))
    {
        Serial.println("Failed to open socket to MQTT server");
        m_Status = MQTT::Status::Error;
        return m_Status;
        
    }
    Serial.println("Opened socket to MQTT server");
    m_Status = MQTT::Status::Connecting;
    this->GenerateConnectPacket();
    m_Client.write(m_Buffer,m_BufferLength);
    return m_Status;
}

void MQTT::MQTTClient::GenerateConnectPacket()
{
    uint8_t *ptr = m_Buffer;


    *(ptr++) = MQTT_PACKET_CONNECT;
    ++ptr; // Skip length field
    ptr = MQTT::UTF8Encode(ptr,"MQTT", 4); // Protocol name
    *(ptr++) = 4; // Protocol level
    *(ptr++) = MQTT_CF_CLEAN_SESSION;
    *(ptr++) = 0;
    *(ptr++) = 255;  // Keep alive 255 secs
    ptr = MQTT::UTF8Encode(ptr,"abcd1234abcd1234", 16);
    m_BufferLength = (ptr - m_Buffer);
    m_Buffer[1] =  m_BufferLength - 2;
}

MQTT::Status MQTT::MQTTClient::Loop()
{
    if (( m_Status == MQTT::Status::Error ) || ( m_Status == MQTT::Status::Disconnected )) return m_Status;
    if (m_Client.available())
    {

    }


}