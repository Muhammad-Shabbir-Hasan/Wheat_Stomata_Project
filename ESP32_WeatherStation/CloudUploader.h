#pragma once

#ifndef CLOUD_UPLOADER_H
#define CLOUD_UPLOADER_H

#define MQQT_BUFFER_SIZE 3000


#include <Arduino.h>
#include "Config.h"

class CloudUploader
{
public:

    //--------------------------------------------------
    // Initialization
    //--------------------------------------------------

    bool begin();

    //--------------------------------------------------
    // MQTT
    //--------------------------------------------------

    bool connectMQTT();

    //--------------------------------------------------
    // Upload Functions
    //--------------------------------------------------

    bool upload(
            String payload);

    bool uploadPending();

    bool uploadImmediately();

    //--------------------------------------------------
    // Device Information
    //--------------------------------------------------

    void setSuitNumber(
            uint8_t suitNumber);

    void setPosition(
            String position);

    String getDeviceName();

private:

    //--------------------------------------------------
    // Device Configuration
    //--------------------------------------------------

    uint8_t envSuitNumber =
        ENV_SUIT_NUMBER;

    String sensorPosition =
        SENSOR_LOCATION;
};

#endif