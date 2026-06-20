#include "CloudUploader.h"

#include <WiFi.h>
#include <PubSubClient.h>

//--------------------------------------------------
// MQTT Client
//--------------------------------------------------

static WiFiClient espClient;




static PubSubClient mqttClient(
    espClient);
//--------------------------------------------------
// Begin
//--------------------------------------------------

bool CloudUploader::begin()
{
    Serial.println(
        "[ThingsBoard] Initializing");

    mqttClient.setServer(
        THINGSBOARD_SERVER,
        1883);

    mqttClient.setBufferSize(MQQT_BUFFER_SIZE);

    Serial.println(
        "[ThingsBoard] Ready");

    return true;
}

//--------------------------------------------------
// Connect MQTT
//--------------------------------------------------

bool CloudUploader::connectMQTT()
{
    if(mqttClient.connected())
    {
        return true;
    }

    Serial.println(
        "[ThingsBoard] Connecting...");

    if(mqttClient.connect(
            getDeviceName().c_str(),
            THINGSBOARD_TOKEN,
            NULL))
    {
        Serial.println(
            "[ThingsBoard] Connected");

        return true;
    }

    Serial.print(
        "[ThingsBoard] Connection Failed. RC = ");

    Serial.println(
        mqttClient.state());

    return false;
}

//--------------------------------------------------
// Upload Telemetry
//--------------------------------------------------

bool CloudUploader::upload(
        String payload)
{
    
    
    if(WiFi.status() != WL_CONNECTED)
    {
        Serial.println(
            "[ThingsBoard] WiFi Not Connected");

        return false;
    }

    Serial.print( "[ThingsBoard] Payload Size = ");

    Serial.println( payload.length());

    mqttClient.loop();

    if(!connectMQTT())
    {
        return false;
    }

    bool result =
        mqttClient.publish(
            "v1/devices/me/telemetry",
            payload.c_str());

    if(result)
    {
        Serial.println(
            "[ThingsBoard] Upload Success");
    }
    else
    {
        Serial.println(
            "[ThingsBoard] Upload Failed");

        if(!result)
        {
            Serial.print(
                "MQTT State = ");

            Serial.println(
                mqttClient.state());
        }
    }

    return result;
}

//--------------------------------------------------
// Upload Pending
//--------------------------------------------------

bool CloudUploader::uploadPending()
{
    return true;
}

//--------------------------------------------------
// Upload Immediately
//--------------------------------------------------

bool CloudUploader::uploadImmediately()
{
    return true;
}

//--------------------------------------------------
// Suit Number
//--------------------------------------------------

void CloudUploader::setSuitNumber(
        uint8_t suitNumber)
{
    envSuitNumber =
        suitNumber;
}

//--------------------------------------------------
// Position
//--------------------------------------------------

void CloudUploader::setPosition(
        String position)
{
    sensorPosition =
        position;
}

//--------------------------------------------------
// Device Name
//--------------------------------------------------

String CloudUploader::getDeviceName()
{
    String name;

    name += "Env_Suit_";
    name += String(envSuitNumber);
    name += "_";
    name += sensorPosition;

    return name;
}