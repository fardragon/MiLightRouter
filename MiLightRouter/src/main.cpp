#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <MQTT.h>
#include "WebPages.h"

#define _CONNECTIONTIMEOUT 10000

bool SoftAP = false;
MQTT::MQTTClient *mqtt = nullptr;
ESP8266WebServer webServer;

bool sent = false;

void setup() 
{

    Serial.begin(115200);    
    Serial.println();

    auto ConnectionTry = millis();
    WiFi.hostname("MiLightRouter");
    WiFi.begin("Livebox-T110E5", "fv215b183");

    Serial.print("Connecting: ");
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - ConnectionTry >= _CONNECTIONTIMEOUT)
        {
            SoftAP = true;
            WiFi.disconnect();
            Serial.println();
            Serial.println("Network error. Entering SoftAP mode.");
            break;
        }
        Serial.print(".");
        delay(500);
    }

    if (SoftAP)
    {
        WiFi.softAPConfig({192,168,1,1},{192,168,1,1},{255,255,255,0});
        WiFi.softAP("MiLightRouter", "12345678");
        webServer.on("/",&WebPages::ConfigRoot);


    }
    else
    {
        Serial.println();
        Serial.print("Connected, IP address: ");
        Serial.println(WiFi.localIP());
        mqtt = new MQTT::MQTTClient();
        mqtt->Initialize( "192.168.1.5" , 1883);
    }
    webServer.begin();   
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

void loop() 
{
    if (SoftAP)
    {
        webServer.handleClient();
    }
    else
    {
        auto result = mqtt->Loop();
        if ((result == MQTT::Status::Connected) && (!sent))
        {
            mqtt->Subscribe("czop", test);
            sent = true;
        }
        webServer.handleClient();
    }
}