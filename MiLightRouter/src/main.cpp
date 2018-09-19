#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <MQTT.h>
#include <WiFiManager.h>
#include <WebHandler.h>
#include <Milight.h>
#include <Settings.h>

#include <string>
#include <vector>
#include <algorithm>
#include <errno.h>

#define RESET_PIN 4

#define NRF_CE 15
#define NRF_CS 5

void message_callback(uint8_t* data, uint8_t length);
void factoryReset();
std::vector<std::string> tokenize_message(char* message);

WiFiManager wifiManager;
 
MQTT::MQTTClient *mqtt = nullptr;
ESP8266WebServer *server = nullptr;
WebHandler *handler = nullptr;
Milight *milight = nullptr;

bool sent = false;

void setup() 
{

    Serial.begin(115200);    
    Serial.println();


    

    pinMode(RESET_PIN, INPUT_PULLUP);
    attachInterrupt(RESET_PIN, factoryReset, FALLING);

    wifiManager.autoConnect("MiLight Router", "12345678");

    mqtt = new MQTT::MQTTClient();
    server = new ESP8266WebServer(80);
    milight = new Milight(NRF_CE, NRF_CS);
    auto ret = milight->init();
    if (ret != 0)
    {
        Serial.printf("Radio initialization failed: %d\n\r", ret);
        server->on("/milight", []()->void {handler->HandleMilightError();});
    }
    else
    {
        server->on("/milight", []()->void {handler->HandleMilight();});
    }
    handler = new WebHandler(server, milight);
    server->on("/", []()-> void { handler->HandleRoot();});
    server->on("/mqtt", []()-> void { handler->HandleMQTTConfig();});
    server->on("/restart", []()-> void { handler->HandleRestart(); delay(100); ESP.restart();});
    server->on("/factory-reset", []()-> void { handler->HandleFactoryRestart(); delay(100); factoryReset();});
    server->on("/milight-cfg", []()->void {handler->HandleMilightCfg();});
    
    server->begin();
    
}





void loop() 
{
    auto result = mqtt->Loop();
    if ((result == MQTT::Status::Connected) && (!sent))
    {
        Serial.println("MQTT connected. Subscribing");
        std::string topic = eeprom_settings.ReadMQTTTopic();
        if (topic.length() > 0)
        {
            mqtt->Subscribe(topic, message_callback);
        }
        else
        {
            Serial.println("Invalid topic");
        }
        sent = true;
    }
    server->handleClient();
    milight->receive();
}


void message_callback(uint8_t* data, uint8_t length)
{
    Serial.print("Received MQTT data: ");
    for (uint8_t i = 0; i < length; ++i)
    {
        Serial.print(static_cast<char>(data[i]));
    }
    Serial.println();

    char * c_str = new char[(length+1)];
    std::copy(data, data+length, c_str);
    c_str[length] = '\0';
    
    auto tokens = tokenize_message(c_str);

    delete[] c_str;

    if (tokens.size() < 1)
    {
        Serial.println("Invalid command received");
        return;
    }
    std::transform(tokens.front().begin(), tokens.front().end(), tokens.front().begin(), ::tolower);

    auto cmd = string_to_command(tokens.front());

    if (cmd == MilightCommand::COLOR)
    {
        if (tokens.size() < 2)
        {
            Serial.println("Invalid command received");
            return;
        }
        char *test_ptr = nullptr;
        errno = 0;
        auto temp = strtoul(tokens[1].c_str(), &test_ptr, 10);
        if ((test_ptr == nullptr) || (*test_ptr != '\0') || (errno == ERANGE))
        {
            Serial.println("Invalid color argument received");
            return;
        }
        uint8_t color = (temp > 255) ? 255 : temp;
        milight->send_command(cmd, color, 0);
    }
    else if (cmd == MilightCommand::BRIGHTNESS)
    {
        if (tokens.size() < 2)
        {
            Serial.println("Invalid command received");
            return;
        }
        char *test_ptr = nullptr;
        errno = 0;
        auto temp = strtoul(tokens[1].c_str(), &test_ptr, 10);
        if ((test_ptr == nullptr) || (*test_ptr != '\0') || (errno == ERANGE))
        {
            Serial.println("Invalid brightness argument received");
            return;
        }
        uint8_t brightness = (temp > 25) ? 25 : temp;
        milight->send_command(cmd, 0, level_to_brightness(brightness));
    }
    else
    {   
        milight->send_command(cmd, 0, 0);
    }
}

std::vector<std::string> tokenize_message(char* message)
{
    std::vector<std::string> ret;
    char *part = nullptr;
    part = strtok(message, " ");
    while (part)
    {
        ret.emplace_back(part);
        part = strtok(nullptr, " ");
    }
    return ret;
}


void factoryReset()
{
    WiFi.disconnect();
    eeprom_settings.WriteServerAddress("0.0.0.0");
    eeprom_settings.WriteServerPort(0);
    eeprom_settings.WriteUseCredentials(false);
    eeprom_settings.WriteMilightDetectedID(0);
    eeprom_settings.WriteMilightDeviceID(0);
    eeprom_settings.WriteMQTTTopic("");
    eeprom_settings.WritePassword("");
    eeprom_settings.WriteUsername("");
    ESP.restart();
}