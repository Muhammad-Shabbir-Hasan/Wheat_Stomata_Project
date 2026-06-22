#pragma once
#pragma once

#ifndef SOLAR_SENSOR_H
#define SOLAR_SENSOR_H

#include <Arduino.h>



#define SOLAR_SERIAL Serial2

#define SOLAR_RX_PIN 16
#define SOLAR_TX_PIN 17

//--------------------------------------------------
// Modbus Command
//--------------------------------------------------

static const uint8_t SOLAR_RADIATION_REQUEST_FRAME[7] =
{
    0x01,
    0x03,
    0x00,
    0x00,
    0x00,
    0x01,
    0x84
};


class SolarSensor
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
    // Get Data
    //--------------------------------------------------

    float getRadiation();
};

#endif
