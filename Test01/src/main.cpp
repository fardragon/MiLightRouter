#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

MQTT::MQTTClient mqtt;

void setup() 
{

    Serial.begin(115200);    
    Serial.println();

    WiFi.hostname("MiLight MQTT Gateway");
    WiFi.begin("Livebox-T110E5", "fv215b183");
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    mqtt.Initialize({192,168,1,23}, 1883);
}

void loop() 
{
    mqtt.Loop();

}