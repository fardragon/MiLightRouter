#ifndef __WEBHANDLER__
#define __WEBHANDLER__

#include <ESP8266WebServer.h>
#include <MQTT.h>
#include <string>
#include <Milight.h>

class WebHandler
{
    public:
    WebHandler(ESP8266WebServer *server, MQTT::MQTTClient *mqtt, Milight *milight);

    void HandleRoot();
    void HandleMQTTConfig();
    void HandleRestart();
    void HandleMilight();
    
    private:
    void SendIndex();
    void SendMQTTConfig();
    void SendRestart();
    void SendMilight();

    void StringReplace(std::string &target, const std::string &remove, const std::string &insert);

    private:
    ESP8266WebServer *m_server;
    MQTT::MQTTClient *m_mqtt;
    Milight *m_milight;
};



#endif
