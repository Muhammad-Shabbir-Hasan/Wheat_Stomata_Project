#pragma once


#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

#define GPS_SERIAL Serial2

#define GPS_RX_PIN 16
#define GPS_TX_PIN 17



//--------------------------------------------------
// Storage
//--------------------------------------------------

static double latitude  = 0.0;
static double longitude = 0.0;

static float hdop = 0.0;
static float vdop = 0.0;




class GPSSensor
{
public:

    //--------------------------------------------------
    // Initialization
    //--------------------------------------------------

    bool begin();

    //--------------------------------------------------
    // Read Latest GPS Data
    //--------------------------------------------------

    bool read();

    //--------------------------------------------------
    // Getters
    //--------------------------------------------------

    double getLat();

    double getLong();

    float getHDOP();

    float getVDOP();

private:

};


#endif
