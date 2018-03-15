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

    mqtt.Initialize( "fardragon.ddns.net" , 1883);
    
}

bool sent = false;

void test(uint8_t* data, uint8_t length)
{
    Serial.println("czop");
    Serial.print("Data: ");
    for (uint8_t i = 0; i < length; ++i)
    {
        Serial.print(data[i], HEX);
    }
    Serial.println();
}

void loop() 
{
    auto result = mqtt.Loop();
    if ((result == MQTT::Status::Connected) && (!sent))
    {
        mqtt.Subscribe("czop", test);
        sent = true;
    }

}