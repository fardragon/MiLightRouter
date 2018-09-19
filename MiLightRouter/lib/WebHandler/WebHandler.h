#ifndef __WEBHANDLER__
#define __WEBHANDLER__

#include <ESP8266WebServer.h>
#include <string>
#include <Milight.h>

class WebHandler
{
    public:
    WebHandler(ESP8266WebServer *server, Milight *milight);

    void HandleRoot();
    void HandleMQTTConfig();
    void HandleRestart();
    void HandleMilight();
    void HandleMilightError();
    void HandleMilightCfg();
    void HandleFactoryRestart();
    
    
    private:
    void SendIndex();
    void SendMQTTConfig();
    void SendRestart();
    void SendMilight();
    void SendMilightErr();
    void SendMilightCfg();

    void StringReplace(std::string &target, const std::string &remove, const std::string &insert);

    private:
    ESP8266WebServer *m_server;
    Milight *m_milight;
};



#endif
