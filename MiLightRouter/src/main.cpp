#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

MQTT::MQTTClient mqtt;

void setup() 
{

    Serial.begin(115200);    
    Serial.println();

    WiFi.hostname("MiLightRouter");
    WiFi.begin("Klejnot Nilu", "fv215b183");
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    mqtt.Initialize({83 , 29, 147, 156} , 1883);
    
}

bool sent = false;

void loop() 
{
    auto result = mqtt.Loop();
    if ((result == MQTT::Status::Connected) && (!sent))
    {
        mqtt.Subscribe("czop", 4);
        sent = true;
    }

}