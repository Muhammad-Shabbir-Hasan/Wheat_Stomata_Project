#pragma once

#include <Arduino.h>
#include "Config.h"



extern const char* SENSOR_NAMES[MUX_CHANNELS];

const int NUM_SAMPLES = 5;

const int SAMPLE_DELAY_MS =
    500 / NUM_SAMPLES;   // 40 ms


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