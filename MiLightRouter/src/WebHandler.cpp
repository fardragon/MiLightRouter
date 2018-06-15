#include "WebHandler.h"


#include <WString.h>


static const char *INDEX_HTML PROGMEM =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>MiLight Router Index</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
"</style>"
"</head>"
"<body>"
"<h1>MiLight Router</h1>"

"<form action=\"/mqtt\">"
    "<input type=\"submit\" value=\"Cofigure MQTT\" />"
"</form>"

"<form action=\"/milight\">"
    "<input type=\"submit\" value=\"Cofigure Milight\" />"
"</form>"

"<form action=\"/restart\">"
    "<input type=\"submit\" value=\"Restart\" />"
"</form>"

"</body>"
"</html>";

static const char *MQTT_HTML PROGMEM =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>MiLight Router MQTT Settings</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
"</style>"
"</head>"
"<body>"
"<h1>MiLight Router</h1>"

"<FORM action=\"/mqtt\" method=\"post\">"
"<P>"
" MQTT server address (max 32 characters):<br>"
"<input type=\"text\" name=\"address\" value=\"__ADDRESS__\"><br>"
"MQTT server port:<br>"
"<input type=\"number\" name=\"port\" value=\"__PORT__\" min=\"1024\" max=\"65535\"><br>"

"<input type=\"radio\" name=\"credentials\" value=\"yes\" __USE__CRED__> Use credentials<br>"
"<input type=\"radio\" name=\"credentials\" value=\"no\" __DONT__USE__CRED> Don't use credentials<br>"

"MQTT Username: <br>"
"<input type=\"text\" name=\"username\" value=\"__USERNAME__\"><br>"

"MQTT Password: <br>"
"<input type=\"password\" name=\"username\" value=\"__PASSWORD__\"><br>"

"<input type=\"submit\" value=\"Send\">"


"</P>"
"</FORM>"
"<form action=\"/\">"
    "<input type=\"submit\" value=\"Back\" />"
"</form>"

"</body>"
"</html>";

static const char *RESTART_HTML PROGMEM =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>MiLight Router Restart</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
"</style>"
"</head>"
"<body>"
"<h1>Device restart</h1>"
"</FORM>"
"<form action=\"/\">"
    "<input type=\"submit\" value=\"Back\" />"
"</form>"

"</body>"
"</html>";

static const char *MILIGHT_HTML =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>MiLight Router Restart</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
"</style>"
"</head>"
"<body>"
"<h1>Milight settings</h1>"

"<FORM action=\"/milight\" method=\"post\">"
"<P>"
"Group select:<br>"

"<input type=\"radio\" name=\"group\" value=\"1\"> 1<br>"
"<input type=\"radio\" name=\"group\" value=\"2\"> 2<br>"
"<input type=\"radio\" name=\"group\" value=\"3\"> 3<br>"
"<input type=\"radio\" name=\"group\" value=\"4\"> 4<br>"

"<input type=\"submit\" value=\"Pair\">"


"</P>"
"</FORM>"

"<form action=\"/\">"
    "<input type=\"submit\" value=\"Back\" />"
"</form>"

"</body>"
"</html>";


WebHandler::WebHandler(ESP8266WebServer *server, MQTT::MQTTClient *mqtt, Milight *milight)
    : m_server(server), m_mqtt(mqtt), m_milight(milight) {};

void WebHandler::HandleRoot()
{
    this->SendIndex();
}

void WebHandler::HandleMQTTConfig()
{
    if (m_server->args() > 0)
    {
        if (m_server->hasArg("address"))
        {
            auto address = m_server->arg("address");
            m_mqtt->WriteServerAddress(address.c_str());
        }

        if (m_server->hasArg("port"))
        {
            auto port_s = m_server->arg("port");
            auto port = std::strtoul(port_s.c_str(),nullptr, 10);
            m_mqtt->WriteServerPort(port);
        }
        if (m_server->hasArg("credentials"))
        {
            auto useCredentials = m_server->arg("credentials");
            if (useCredentials == "yes")
            {
                m_mqtt->WriteUseCredentials(true);
            }   
            else if (useCredentials == "no")
            {
                m_mqtt->WriteUseCredentials(false);
            }
        }
        if (m_server->hasArg("username"))
        {
            std::string temp = m_server->arg("username").c_str();
            m_mqtt->WriteUsername(temp);
        }
        if (m_server->hasArg("password"))
        {
            std::string temp = m_server->arg("password").c_str();
            m_mqtt->WriteUsername(temp);
        }
    }
    this->SendMQTTConfig();
}

void WebHandler::HandleRestart()
{
    this->SendRestart();
}

void WebHandler::HandleMilight()
{
    if (m_server->args() > 0)
    {
        if (m_server->hasArg("group"))
        {
            std::string temp = m_server->arg("group").c_str();
            Serial.println(temp.c_str());
            auto group_s = m_server->arg("group");
            auto group = std::strtoul(group_s.c_str(), nullptr, 10);
            if (group == 1) for (uint8_t i = 0; i < 5; ++i) m_milight->TestOn();   
            if (group == 2) for (uint8_t i = 0; i < 5; ++i) m_milight->TestOff();   
                
        }
    }
    this->SendMilight();
}

void WebHandler::SendIndex()
{
    m_server->send(200, "text/html", FPSTR(INDEX_HTML));
}

void WebHandler::SendMilight()
{
    m_server->send(200, "text/html", FPSTR(MILIGHT_HTML));
}

void WebHandler::SendMQTTConfig()
{
    std::string page (String(FPSTR(MQTT_HTML)).c_str());
    this->StringReplace(page, "__ADDRESS__", m_mqtt->ReadServerAddress());

    char buffer[5];
    sprintf(buffer, "%d", m_mqtt->ReadServerPort());

    this->StringReplace(page, "__PORT__", std::string(buffer));

    bool use_cred = m_mqtt->ReadUseCredentials();
    if (use_cred)
    {
        this->StringReplace(page, "__USE__CRED__", "checked");
        this->StringReplace(page, "__DONT__USE__CRED", "");
    }
    else
    {
        this->StringReplace(page, "__USE__CRED__", "");
        this->StringReplace(page, "__DONT__USE__CRED", "checked");
    }

    this->StringReplace(page, "__USERNAME__", m_mqtt->ReadUsername());
    this->StringReplace(page, "__PASSWORD__", m_mqtt->ReadPassword());

    m_server->send(200, "text/html", page.c_str());
}


void WebHandler::StringReplace(std::string &target, const std::string &remove, const std::string &insert)
{
    std::size_t pos;
    while ((pos = target.find(remove)) != std::string::npos)
    {
        target.replace(pos, remove.length(), insert);
    }
}

void WebHandler::SendRestart()
{
    m_server->send(200, "text/html", FPSTR(RESTART_HTML));
}