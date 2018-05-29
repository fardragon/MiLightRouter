#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <MQTT.h>
#include <WifiManager.h>

#include <Rf24.h>

#include "WebHandler.h"

#define RESET_PIN 4

void test(uint8_t* data, uint8_t length);
void factoryReset();

WiFiManager wifiManager;
 
MQTT::MQTTClient *mqtt = nullptr;
ESP8266WebServer *server = nullptr;
WebHandler *handler = nullptr;
RF24 radio(15,5);

bool sent = false;

void setup() 
{

    Serial.begin(115200);    
    Serial.println();

    if (radio.begin())
    {
        Serial.println("Działa");
    }
    else
    {
        Serial.println("Nie działa");
    }

    pinMode(RESET_PIN, INPUT_PULLUP);
    attachInterrupt(RESET_PIN, factoryReset, FALLING);

    wifiManager.autoConnect("MiLight Router", "12345678");

    mqtt = new MQTT::MQTTClient();
    server = new ESP8266WebServer(80);
    handler = new WebHandler(server, mqtt);

    server->on("/", []()-> void { handler->HandleRoot();});
    server->on("/mqtt", []()-> void { handler->HandleMQTTConfig();});
    server->on("/restart", []()-> void { handler->HandleRestart(); ESP.restart();});

    server->begin();
}





void loop() 
{

    auto result = mqtt->Loop();
    if ((result == MQTT::Status::Connected) && (!sent))
    {
        mqtt->Subscribe("czop", test);
        sent = true;
    }
    server->handleClient();
}


void test(uint8_t* data, uint8_t length)
{
    Serial.println("czop");
    Serial.print("Data: ");
    for (uint8_t i = 0; i < length; ++i)
    {
        Serial.print(static_cast<char>(data[i]));
    }
    Serial.println();
}

void factoryReset()
{
    WiFi.disconnect();
    ESP.restart();
}