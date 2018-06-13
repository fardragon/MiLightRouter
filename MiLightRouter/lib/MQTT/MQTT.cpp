#include "MQTT.h"
#include "Utility.h"

#include <functional>
#include <algorithm>
#include <EEPROM.h>

MQTT::MQTTClient::MQTTClient()
    : m_Status(MQTT::Status::Disconnected), m_LastActivity(millis()), m_PacketID(0), m_SentPing(false)
{
    EEPROM.begin(48);

    auto address = this->ReadServerAddress().c_str();
    auto port = this->ReadServerPort();
    
    this->Initialize(address, port);

};

void MQTT::MQTTClient::Initialize(const char *ServerAddress, uint16_t Port)
{
    if(!m_Client.connect(ServerAddress,Port))
    {
        Serial.println("Failed to open socket to MQTT server");
        m_Status = MQTT::Status::Error;
        return;
        
    }
    Serial.println("Opened socket to MQTT server");
    m_Status = MQTT::Status::Connecting;
    this->GenerateConnectPacket();
    this->SendData();
}

void MQTT::MQTTClient::GeneratePingPacket()
{
    uint8_t *ptr = m_Buffer;
    *(ptr++) = MQTT_PACKET_PINGREQ;
    ++ptr; // Skip length field
    m_BufferLength = (ptr - m_Buffer);
    m_Buffer[1] =  m_BufferLength - 2;
}

void MQTT::MQTTClient::GenerateConnectPacket()
{
    uint8_t *ptr = m_Buffer;
    *(ptr++) = MQTT_PACKET_CONNECT;
    ++ptr; // Skip length field
    ptr = MQTT::UTF8Encode(ptr,"MQTT", 4); // Protocol name
    *(ptr++) = 4; // Protocol level
    *(ptr++) = MQTT_CF_CLEAN_SESSION;
    *(ptr++) = (MQTT_KEEP_ALIVE & 0xFF00) >> 8;   // Keep alive
    *(ptr++) = (MQTT_KEEP_ALIVE & 0x00FF);  // Keep alive
    ptr = MQTT::UTF8Encode(ptr,"abcd1234abcd1234", 16);
    m_BufferLength = (ptr - m_Buffer);
    m_Buffer[1] =  m_BufferLength - 2;
}

uint16_t MQTT::MQTTClient::GenerateSubscribePacket(const char *topic, uint8_t length)
{
    uint8_t *ptr = m_Buffer;
    *(ptr++) = MQTT_PACKET_SUBSCRIBE;
    ++ptr; // Skip length field
    auto packetID = this->NextPacketID();
    *(ptr++) = (packetID & 0xFF00) >> 8;
    *(ptr++) = (packetID & 0x00FF);
    ptr = MQTT::UTF8Encode(ptr, topic, length);
    *(ptr++) = 1;
    m_BufferLength = (ptr - m_Buffer);
    m_Buffer[1] =  m_BufferLength - 2;
    return packetID;
}

void MQTT::MQTTClient::GeneratePublishAckPacket(const uint16_t &packetID)
{
    m_Buffer[0] = MQTT_PACKET_PUBACK;
    m_Buffer[1] = 2;
    m_Buffer[2] = (packetID & 0xFF00) >> 8;
    m_Buffer[3] = (packetID & 0x00FF);
    m_BufferLength = 4;
}

MQTT::Status MQTT::MQTTClient::Loop()
{
    if (( m_Status == MQTT::Status::Error ) || ( m_Status == MQTT::Status::Disconnected )) return m_Status;
    if (m_Client.available())
    {
        if (!this->ReceivePacket())
        {
            m_Client.stop();
            m_Status = MQTT::Status::Error;
            Serial.println("Packet reception error");
            return m_Status;
        }
        this->InterpretPacket();

    }
    uint32_t current = millis();
    if ((current - m_LastActivity) > MQTT_KEEP_ALIVE * 750)
    {
        if (m_SentPing)
        {
            m_Client.stop();
            m_Status = MQTT::Status::Error;
            Serial.println("MQTT Server Timeout");
            return m_Status;
        }
        this->GeneratePingPacket();
        this->SendData();
        m_SentPing = true;      
    }
    return m_Status;
}

bool MQTT::MQTTClient::ReceivePacket()
{
    uint16_t length = 0;
    if (!this->ReadByte(m_Buffer,&length)) return false;

    uint32_t recLength = 0;
    uint32_t multiplier = 1;

    uint8_t byte;
    do
    {
        if (length > 4) return false;
        if (!this->ReadByte(&byte)) return false;
        m_Buffer[length++] = byte;
        recLength += (byte & 127) * multiplier;
        multiplier *= 128;
    } while ( (byte & 128 ) != 0);

    for (uint32_t i = 0; i < recLength; ++i)
    {
        if(!this->ReadByte(&byte)) return false;
        if (length >= MQTT_MAX_PACKET_SIZE) continue;
        m_Buffer[length++] = byte;
    }
    m_BufferLength = length;
    if (length < MQTT_MAX_PACKET_SIZE) return true;
    return false;
}

bool MQTT::MQTTClient::ReadByte(uint8_t *dest, uint16_t *offset)
{
    if ( this->ReadByte(dest + (*offset)) )
    {
        ++(*offset);
        return true;
    }
    return false;
}

bool MQTT::MQTTClient::ReadByte(uint8_t *dest)
{
    uint32_t start = millis();
    while(!m_Client.available())
    {
        uint32_t current = millis();
        if ((current - start) >= MQTT_TIMEOUT)
        {
            return false;
        }
    }
    m_Client.readBytes(dest,1);
    return true;
}

void MQTT::MQTTClient::SendData()
{
    m_Client.write(m_Buffer,m_BufferLength);
    m_LastActivity = millis();
    /*
    Serial.println("New packet: ");
    for (uint8_t i = 0; i < m_BufferLength; ++i)
    {
        Serial.println(m_Buffer[i]);
    }
    */
}

bool MQTT::MQTTClient::InterpretPacket()
{
    uint8_t PacketType = m_Buffer[0] & 0xF0;
    switch (PacketType)
    {
        case MQTT_PACKET_CONNACK:
        return this->Packet_CONNACK();
        
        case MQTT_PACKET_PINGRESP:
        return this->Packet_PINGRESP();

        case MQTT_PACKET_SUBACK:
        return this->Packet_SUBACK();

        case MQTT_PACKET_PUBLISH:
        return this->Packet_PUBLISH();




        default:
        Serial.print("Packet type error: ");
        Serial.println(PacketType, BIN);
        m_Status = MQTT::Status::Error;
        m_Client.stop();
        return false;
    }
}

uint8_t MQTT::MQTTClient::LengthBytes()
{
    if (m_BufferLength <= 127) return 1;
    if (m_BufferLength <= 16383) return 2;
    if (m_BufferLength <= 2097151) return 3;
    return 4;
}

bool MQTT::MQTTClient::Packet_CONNACK()
{
    if ((m_BufferLength != 4) || (m_Buffer[1] != 2)) 
    {
        m_Status = MQTT::Status::Error;
        m_Client.stop();
        Serial.println("CONNACK Length error");
        return false;
    }
    uint8_t ConnectFlags = m_Buffer[2];
    if (ConnectFlags != 0)
    {
        m_Status = MQTT::Status::Error;
        m_Client.stop();
        Serial.println("CONNACK Connect flags error");
        return false;
    }
    uint8_t ConnectReturn = m_Buffer[3];
    switch (ConnectReturn)
    {
        case 0:
        m_Status = MQTT::Status::Connected;
        Serial.println("Connected to MQTT server");
        return true;

        case 1:
        m_Status = MQTT::Status::Error;
        Serial.println("MQTT version error");
        m_Client.stop();
        return false;

        case 2:
        m_Status = MQTT::Status::Error;
        Serial.println("MQTT Rejected ID");
        m_Client.stop();
        return false;

        case 3:
        m_Status = MQTT::Status::Error;
        Serial.println("MQTT server unavalaible");
        m_Client.stop();
        return false;

        case 4:
        m_Status = MQTT::Status::Error;
        Serial.println("MQTT Invalid login data");
        m_Client.stop();
        return false;

        case 5:
        m_Status = MQTT::Status::Error;
        Serial.println("MQTT Not authorized");
        m_Client.stop();
        return false;

        default:
        m_Status = MQTT::Status::Error;
        Serial.println("MQTT Invalid return code");
        m_Client.stop();
        return false;
    }

}

bool MQTT::MQTTClient::Packet_PINGRESP()
{
    if ((m_BufferLength != 2) || (m_Buffer[1] != 0)) 
    {
        m_Status = MQTT::Status::Error;
        m_Client.stop();
        Serial.println("PINGRESP Length error");
        return false;
    }
    Serial.println("Server ping response");
    m_SentPing = false;
    return true;
}

bool MQTT::MQTTClient::Packet_SUBACK()
{
    Serial.println("SUBACK");
    uint8_t startIndex = this->LengthBytes() + 1;
    uint16_t packetID = (m_Buffer[startIndex] << 8) | (m_Buffer[startIndex+1]);
    Serial.print("Packet ID: ");
    Serial.println(packetID);
    auto pred = [packetID](const Subscribtion &a) -> bool 
    {
        return (a.m_ID == packetID);
    };
    auto ix = std::find_if(m_Subscriptions.begin(),m_Subscriptions.end(),pred);
    if (ix == m_Subscriptions.end()) return false;
    (*ix).m_Ack = true;
    return true;
}

bool MQTT::MQTTClient::Packet_PUBLISH()
{
    uint8_t qos = ((*m_Buffer) & 0b00000110) >> 1;
    if ((qos != 1) && (qos != 0)) 
    {
        Serial.print ("Invalid QOS: ");
        Serial.println(qos);
        m_Client.stop();
        m_Status = MQTT::Status::Error;
        return false;
    }
    uint8_t skip = this->LengthBytes();
    uint8_t* ptr = m_Buffer + skip + 1;
    std::string topic;
    ptr = MQTT::UTF8Decode(ptr, topic);

    auto pred = [topic](const Subscribtion &a) -> bool 
    {
        return (a.m_Topic == topic);
    };
    auto ix = std::find_if(m_Subscriptions.begin(),m_Subscriptions.end(),pred);
    if (ix == m_Subscriptions.end()) 
    {
        Serial.print("Invalid topic: ");
        Serial.println(topic.c_str());
        m_Client.stop();
        m_Status = MQTT::Status::Error;
        return false;
    }
    if (qos == 1)
    {
        uint16_t packetID = (*(ptr++)) << 8;
        packetID |= *(ptr++);

        uint8_t payloadLength = m_BufferLength - (ptr - m_Buffer);
        uint8_t *payload = new uint8_t[payloadLength];
        memcpy(payload, ptr, payloadLength);  
        (*ix).m_Handler(payload,payloadLength);
        delete[] payload;
        this->GeneratePublishAckPacket(packetID);
        this->SendData();
        return true;
    }
    else
    {
        uint8_t payloadLength = m_BufferLength - (ptr - m_Buffer);
        uint8_t *payload = new uint8_t[payloadLength];
        memcpy(payload, ptr, payloadLength);
        (*ix).m_Handler(payload,payloadLength);
        delete[] payload;
        return true;
    }   
}

inline uint16_t MQTT::MQTTClient::NextPacketID()
{
    return ++m_PacketID;
}

void MQTT::MQTTClient::Subscribe(std::string topic, std::function<void(uint8_t*,uint8_t)> handler)
{
    if (m_Status != MQTT::Status::Connected)
    {
        Serial.println("Cannot subscribe at this moment");
    }
    auto packetID = this->GenerateSubscribePacket(topic.c_str(),topic.length());
    m_Subscriptions.emplace_back(topic,handler,packetID);
    this->SendData();
}

std::string MQTT::MQTTClient::ReadServerAddress()
{
    EEPROM.begin(MQTT_SERVER_ADDRESS + MQTT_SERVER_SIZE);

    std::string result;
    for (uint8_t i = 0; i < MQTT_SERVER_SIZE; ++i)
    {
        auto byte = EEPROM.read(MQTT_SERVER_ADDRESS + i);
        if (byte == '\0') break;
        result +=byte;
    }

    EEPROM.end();
    return result;
}

void MQTT::MQTTClient::WriteServerAddress(const std::string &address)
{
    EEPROM.begin(MQTT_SERVER_ADDRESS + MQTT_SERVER_SIZE);
    for (uint8_t i = 0; i < MQTT_SERVER_SIZE; ++i)
    {
        if (i < address.length())
        {
            EEPROM.write(MQTT_SERVER_ADDRESS + i, address[i]);
        }
        else
        {
            EEPROM.write(MQTT_SERVER_ADDRESS + i, '\0');
        }
    }
    EEPROM.end();
}

 uint16_t MQTT::MQTTClient::ReadServerPort()
 {
     EEPROM.begin(MQTT_PORT_ADDRESS + MQTT_PORT_SIZE);
     uint16_t result;
     EEPROM.get(MQTT_PORT_ADDRESS, result);
     EEPROM.end();
     return result;
 }

  void MQTT::MQTTClient::WriteServerPort(uint16_t port)
  {
     EEPROM.begin(MQTT_PORT_ADDRESS + MQTT_PORT_SIZE);
     EEPROM.put(MQTT_PORT_ADDRESS, port);
     EEPROM.end();
  }

  bool MQTT::MQTTClient::ReadUseCredentials()
  {
      EEPROM.begin(MQTT_USE_CREDENTIALS_ADDRESS + MQTT_USE_CREDENTIALS_SIZE);
      bool result;
      EEPROM.get(MQTT_USE_CREDENTIALS_ADDRESS, result);
      EEPROM.end();
      return result;
  }

  void MQTT::MQTTClient::WriteUseCredentials(bool use)
  {
      EEPROM.begin(MQTT_USE_CREDENTIALS_ADDRESS + MQTT_USE_CREDENTIALS_SIZE);
      EEPROM.put(MQTT_USE_CREDENTIALS_ADDRESS, use);
      EEPROM.end();
  }

  std::string MQTT::MQTTClient::ReadUsername()
  {
      EEPROM.begin(MQTT_USERNAME_ADDRESS+MQTT_USERNAME_SIZE);
      std::string result;
      for (uint8_t i = 0; i < MQTT_USERNAME_SIZE; ++i)
      {
          auto byte = EEPROM.read(MQTT_USERNAME_ADDRESS + i);
          if (byte == '\0') break;
          result += byte;
      }
      EEPROM.end();
      return result;
  }

  void MQTT::MQTTClient::WriteUsername(const std::string &username)
  {
    EEPROM.begin(MQTT_USERNAME_ADDRESS+MQTT_USERNAME_SIZE);
    for (uint8_t i = 0; i < MQTT_USERNAME_SIZE; ++i)
    {
        if (i < username.length())
        {
            EEPROM.write(MQTT_USERNAME_ADDRESS + i, username[i]);
        }
        else
        {
            EEPROM.write(MQTT_USERNAME_ADDRESS + i, '\0');
        }
    }
    EEPROM.end();
  }

   std::string MQTT::MQTTClient::ReadPassword()
   {
       EEPROM.begin(MQTT_PASSWORD_ADDRESS + MQTT_PASSWORD_SIZE);
       std::string result;
       for (uint8_t i = 0; i < MQTT_PASSWORD_SIZE; ++i)
       {
           auto byte = EEPROM.read(MQTT_PASSWORD_ADDRESS + i);
           if (byte == '\0') break;
           result += byte;
       }
       return result;
   }

   void MQTT::MQTTClient::WritePassword(const std::string &password)
   {
    EEPROM.begin(MQTT_PASSWORD_ADDRESS+MQTT_PASSWORD_SIZE);
    for (uint8_t i = 0; i < MQTT_PASSWORD_SIZE; ++i)
    {
        if (i < password.length())
        {
            EEPROM.write(MQTT_PASSWORD_ADDRESS + i, password[i]);
        }
        else
        {
            EEPROM.write(MQTT_PASSWORD_ADDRESS + i, '\0');
        }
    }
    EEPROM.end();
   }