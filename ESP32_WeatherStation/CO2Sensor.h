#pragma once

#ifndef CO2_SENSOR_H
#define CO2_SENSOR_H

#include <Arduino.h>


#define CO2_SERIAL Serial2
#define CO2_RX_PIN 16
#define CO2_TX_PIN 17

//#define CO2_SERIAL Serial0
//#define CO2_RX_PIN 3
//#define CO2_TX_PIN 1



//--------------------------------------------------
// Modbus Command
//--------------------------------------------------

static const uint8_t CO2_REQUEST_FRAME[9] =
{
    0xFF,
    0x01,
    0x86,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x79
};


/*
static const uint8_t CO2_REQUEST_FRAME[9] =
{
    0xFF,
    0x86,
    0x01,
    0xF4,
    0x00,
    0x00,
    0x00,
    0x00,
    0x49
};
*/

class CO2Sensor
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

    int getPPM();
};

#endif
