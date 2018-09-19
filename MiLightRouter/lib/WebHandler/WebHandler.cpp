#include "WebHandler.h"


#include <WString.h>
#include <Settings.h>

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
    "<input type=\"submit\" value=\"Control Milight\" />"
"</form>"

"<form action=\"/restart\">"
    "<input type=\"submit\" value=\"Restart\" />"
"</form>"

"<form action=\"/factory-reset\">"
    "<input type=\"submit\" value=\"Factory reset\" />"
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

"<FORM action=\"/mqtt\" method=\"post\" accept-charset=\"UTF-8\">"
"<P>"
" MQTT server address (max 32 characters):<br>"
"<input type=\"text\" name=\"address\" value=\"__ADDRESS__\"><br>"
"MQTT server port:<br>"
"<input type=\"number\" name=\"port\" value=\"__PORT__\" min=\"1024\" max=\"65535\"><br>"

"<input type=\"radio\" name=\"credentials\" value=\"yes\" __USE__CRED__> Use credentials<br>"
"<input type=\"radio\" name=\"credentials\" value=\"no\" __DONT__USE__CRED> Don't use credentials<br>"

"MQTT Username: <br>"
"<input type=\"text\" name=\"username\" value=\"__USERNAME__\" maxlength=\"__USR_SIZE__\"><br>"

"MQTT Password: <br>"
"<input type=\"password\" name=\"password\" value=\"__PASSWORD__\" maxlength=\"__PWD_SIZE__\"><br>"

"MQTT Topic: <br>"
"<input type=\"text\" name=\"topic\" value=\"__TOPIC__\" maxlength=\"__TPC_SIZE__\"><br>"

"<input type=\"submit\" value=\"Save\">"


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
"<title>MiLight Control</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
"</style>"
"</head>"
"<body>"
"<h1>Milight control</h1>"

"<FORM action=\"/milight\" method=\"post\">"
"<P>"
"Button select:<br>"


"<input type=\"radio\" name=\"button\" value=\"1\"> All ON<br>"
"<input type=\"radio\" name=\"button\" value=\"2\"> All OFF<br>"
"<input type=\"radio\" name=\"button\" value=\"16\"> All WHITE<br>"
"<input type=\"radio\" name=\"button\" value=\"3\"> Group 1 ON<br>"
"<input type=\"radio\" name=\"button\" value=\"4\"> Group 1 OFF<br>"
"<input type=\"radio\" name=\"button\" value=\"21\"> Group 1 PAIR<br>"
"<input type=\"radio\" name=\"button\" value=\"17\"> Group 1 UNAPIR/WHITE<br>"
"<input type=\"radio\" name=\"button\" value=\"5\"> Group 2 ON<br>"
"<input type=\"radio\" name=\"button\" value=\"6\"> Group 2 OFF<br>"
"<input type=\"radio\" name=\"button\" value=\"22\"> Group 2 PAIR<br>"
"<input type=\"radio\" name=\"button\" value=\"18\"> Group 2 UNAPIR/WHITE<br>"
"<input type=\"radio\" name=\"button\" value=\"7\"> Group 3 ON<br>"
"<input type=\"radio\" name=\"button\" value=\"8\"> Group 3 OFF<br>"
"<input type=\"radio\" name=\"button\" value=\"23\"> Group 3 PAIR<br>"
"<input type=\"radio\" name=\"button\" value=\"19\"> Group 3 UNAPIR/WHITE<br>"
"<input type=\"radio\" name=\"button\" value=\"9\"> Group 4 ON<br>"
"<input type=\"radio\" name=\"button\" value=\"10\"> Group 4 OFF<br>"
"<input type=\"radio\" name=\"button\" value=\"24\"> Group 4 PAIR<br>"
"<input type=\"radio\" name=\"button\" value=\"20\"> Group 4 UNAPIR/WHITE<br>"
"<input type=\"radio\" name=\"button\" value=\"11\"> Speed plus<br>"
"<input type=\"radio\" name=\"button\" value=\"12\"> Speed minus<br>"
"<input type=\"radio\" name=\"button\" value=\"13\"> Mode<br>"
"<input type=\"radio\" name=\"button\" value=\"14\"> Brightness<br>"
"<input type=\"radio\" name=\"button\" value=\"15\"> Colour<br>"

"Color:<br>"
"<input type=\"number\" name=\"color\" value=\"0\" min=\"0\" max=\"255\"><br>"

"<input type=\"submit\" value=\"Send\">"


"</P>"
"</FORM>"

"<form action=\"/\">"
    "<input type=\"submit\" value=\"Back\" />"
"</form>"

"</body>"
"</html>";



WebHandler::WebHandler(ESP8266WebServer *server, Milight *milight)
    : m_server(server), m_milight(milight) {};

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
            eeprom_settings.WriteServerAddress(address.c_str());
        }

        if (m_server->hasArg("port"))
        {
            auto port_s = m_server->arg("port");
            auto port = std::strtoul(port_s.c_str(),nullptr, 10);
            if (port != 0) eeprom_settings.WriteServerPort(port);
        }
        if (m_server->hasArg("credentials"))
        {
            auto useCredentials = m_server->arg("credentials");
            if (useCredentials == "yes")
            {
                eeprom_settings.WriteUseCredentials(true);
            }   
            else if (useCredentials == "no")
            {
                eeprom_settings.WriteUseCredentials(false);
            }
        }
        if (m_server->hasArg("username"))
        {
            std::string temp = m_server->arg("username").c_str();
            eeprom_settings.WriteUsername(temp);
        }
        if (m_server->hasArg("password"))
        {
            std::string temp = m_server->arg("password").c_str();
            eeprom_settings.WriteUsername(temp);
        }
        if (m_server->hasArg("topic"))
        {
            std::string temp = m_server->arg("topic").c_str();
            eeprom_settings.WriteMQTTTopic(temp);
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
        if (m_server->hasArg("button"))
        {
            auto button_s = m_server->arg("button");
            auto button = std::strtoul(button_s.c_str(), nullptr, 10);
            MilightCommand cmd = static_cast<MilightCommand>(button);

            if (cmd == MilightCommand::COLOR)
            {
                if (m_server->hasArg("color"))
                {
                    auto color_s = m_server->arg("color");
                    auto color = std::strtoul(color_s.c_str(), nullptr, 10);
                    if (color != 0) m_milight->send_command(cmd, color, 0);
                }
            }
            else if (cmd == MilightCommand::BRIGHTNESS)
            {

            }
            else
            {
                m_milight->send_command(cmd, 0, 0);
            }
            
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
    this->StringReplace(page, "__ADDRESS__", eeprom_settings.ReadServerAddress());

    char buffer[5];
    sprintf(buffer, "%d", eeprom_settings.ReadServerPort());

    this->StringReplace(page, "__PORT__", std::string(buffer));

    bool use_cred = eeprom_settings.ReadUseCredentials();
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

    this->StringReplace(page, "__USERNAME__", eeprom_settings.ReadUsername());
    this->StringReplace(page, "__PASSWORD__", eeprom_settings.ReadPassword());
    this->StringReplace(page, "__TOPIC__", eeprom_settings.ReadMQTTTopic());
    //TODO
    //this->StringReplace(page, "__USR_SIZE__", MQTT_USERNAME_SIZE);

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