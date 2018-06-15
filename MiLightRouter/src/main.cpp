#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <MQTT.h>
#include <WiFiManager.h>

#include <RF24.h>

#include "WebHandler.h"

#define RESET_PIN 4

#define NRF_CE 15
#define NRF_CS 5

void test(uint8_t* data, uint8_t length);
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
    milight = new Milight(NRF_CE, NRF_CS);
    handler = new WebHandler(server, mqtt, milight);

    server->on("/", []()-> void { handler->HandleRoot();});
    server->on("/mqtt", []()-> void { handler->HandleMQTTConfig();});
    server->on("/restart", []()-> void { handler->HandleRestart(); delay(25); ESP.restart();});
    server->on("/milight", []()->void {handler->HandleMilight();});

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
    if (milight->Avalaible()) milight->Receive();
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
    mqtt->WriteServerAddress("0.0.0.0");
    mqtt->WriteServerPort(0);
    mqtt->WriteUseCredentials(false);
    ESP.restart();
}