#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <MQTT.h>
#include <WiFiManager.h>
#include <WebHandler.h>
#include <Milight.h>
#include <Settings.h>
#include <sstream>

#define RESET_PIN 4

#define NRF_CE 15
#define NRF_CS 5

void message_callback(uint8_t* data, uint8_t length);
void factoryReset();

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
    milight = new Milight(NRF_CE, NRF_CS, 0x35D5);
    auto ret = milight->init();
    Serial.printf("Ret: %d\n\r", ret);
    handler = new WebHandler(server, milight);

    server->on("/", []()-> void { handler->HandleRoot();});
    server->on("/mqtt", []()-> void { handler->HandleMQTTConfig();});
    server->on("/restart", []()-> void { handler->HandleRestart(); delay(25); ESP.restart();});
    server->on("/factory-reset", []()-> void { handler->HandleRestart(); delay(25); factoryReset();});
    server->on("/milight", []()->void {handler->HandleMilight();});

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
    memcpy(c_str, data, length);
    c_str[length] = '\0';
    
    char *part = nullptr;
    part = strtok(c_str, " ");
    //TODO
    while (part)
    {
        Serial.println(part);
        part = strtok(nullptr, " ");
    }


}

void factoryReset()
{
    WiFi.disconnect();
    eeprom_settings.WriteServerAddress("0.0.0.0");
    eeprom_settings.WriteServerPort(0);
    eeprom_settings.WriteUseCredentials(false);
    ESP.restart();
}