#pragma once

#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>
#include <DHT.h>

#ifndef DHT_PIN
#define DHT_PIN 14
#endif

#define DHT_TYPE DHT11

class DHTSensor
{
public:

    //--------------------------------------------------
    // Initialization
    //--------------------------------------------------

    bool begin();

    //--------------------------------------------------
    // Read Sensor
    //--------------------------------------------------

    bool read();

    //--------------------------------------------------
    // Getters
    //--------------------------------------------------

    float getTemperature();

    float getHumidity();

private:

    DHT dht =
        DHT(DHT_PIN,
            DHT_TYPE);

    float temperature = 0.0;

    float humidity = 0.0;
};

#endif