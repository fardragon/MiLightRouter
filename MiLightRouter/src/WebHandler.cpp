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
"<INPUT type=\"submit\" value=\"Send\">"
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


WebHandler::WebHandler(ESP8266WebServer *server, MQTT::MQTTClient *mqtt)
    : m_server(server), m_mqtt(mqtt) {};

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
    }
    this->SendMQTTConfig();
}

void WebHandler::HandleRestart()
{
    this->SendRestart();
}

void WebHandler::SendIndex()
{
    m_server->send(200, "text/html", FPSTR(INDEX_HTML));
}

void WebHandler::SendMQTTConfig()
{
    std::string page (String(FPSTR(MQTT_HTML)).c_str());
    this->StringReplace(page, "__ADDRESS__", m_mqtt->ReadServerAddress());

    char buffer[5];
    sprintf(buffer, "%d", m_mqtt->ReadServerPort());

    this->StringReplace(page, "__PORT__", std::string(buffer));
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