#include "DHT11Sensor.h"

bool DHTSensor::begin()
{
    dht.begin();

    delay(1000);

    return true;
}

bool DHTSensor::read()
{
    //--------------------------------------------------
    // Read Humidity
    //--------------------------------------------------

    humidity =
        dht.readHumidity();

    //--------------------------------------------------
    // Read Temperature
    //--------------------------------------------------

    temperature =
        dht.readTemperature();

    //--------------------------------------------------
    // Check Valid
    //--------------------------------------------------

    if(isnan(humidity) ||
       isnan(temperature))
    {
        Serial.println(
            "[DHT11] Read Failed");

        return false;
    }

    //--------------------------------------------------
    // Display
    //--------------------------------------------------

    Serial.print(
        "[DHT11] Temperature = ");

    Serial.print(
        temperature);

    Serial.println(
        " C");

    Serial.print(
        "[DHT11] Humidity = ");

    Serial.print(
        humidity);

    Serial.println(
        " %");

    return true;
}

float DHTSensor::getTemperature()
{
    return temperature;
}

float DHTSensor::getHumidity()
{
    return humidity;
}