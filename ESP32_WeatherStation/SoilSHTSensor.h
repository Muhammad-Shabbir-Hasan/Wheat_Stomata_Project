#pragma once

#ifndef SOIL_SHT_SENSOR_H
#define SOIL_SHT_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

#define I2C_SDA_PIN     21
#define I2C_SCL_PIN     19


class SoilSHTSensor
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

    Adafruit_SHT31 sht31;

    float soilTemperature = 0.0;

    float soilHumidity = 0.0;
};

#endif