#pragma once

#include <Arduino.h>
#include "Config.h"


struct MuxData
{
    float values[MUX_CHANNELS];
};

class MuxManager
{
public:

    bool begin();

    bool selectChannel(uint8_t channel);

    float readChannel(uint8_t channel);

    bool readAllChannels();

    float getSensorValue(uint8_t channel);

    MuxData getAllSensorValues();

private:

    float sensorValues[MUX_CHANNELS];
};