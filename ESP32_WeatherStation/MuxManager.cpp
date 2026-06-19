#include <Arduino.h>
#include "MuxManager.h"

#include "Config.h"


// =============================
// Sensor Scaling
// =============================

// Example values.
// Change according to actual sensor ranges.

const float SENSOR_MIN[MUX_CHANNELS] =
{
    0.0,    // CH0
    0.0,    // CH1
    0.0,    // CH2
    0.0,     // CH3
    0.0,    // CH0
    0.0,    // CH1
    0.0,    // CH2
    0.0,     // CH3
    0.0,    // CH0
    0.0,    // CH1
    0.0,    // CH2
    0.0,     // CH3
    0.0,    // CH0
    0.0,    // CH1
    0.0,    // CH2
    0.0     // CH3
};

const float SENSOR_MAX[MUX_CHANNELS] =
{
    1000.0, // CO
    5000.0, // CO2
    100.0,  // NH3
    100.0,   // H2
    1000.0, // CO
    5000.0, // CO2
    100.0,  // NH3
    100.0,   // H2
    1000.0, // CO
    5000.0, // CO2
    100.0,  // NH3
    100.0,   // H2
    1000.0, // CO
    5000.0, // CO2
    100.0,  // NH3
    100.0   // H2
};

bool MuxManager::begin()
{
    pinMode(MUX_EN, OUTPUT);

    pinMode(MUX_S0, OUTPUT);
    pinMode(MUX_S1, OUTPUT);
    pinMode(MUX_S2, OUTPUT);
    pinMode(MUX_S3, OUTPUT);

    digitalWrite(MUX_EN, LOW);

    return true;
}

bool MuxManager::selectChannel(uint8_t channel)
{
    if(channel > 15)
        return false;

    digitalWrite(MUX_S0, channel & 0x01);
    digitalWrite(MUX_S1, (channel >> 1) & 0x01);
    digitalWrite(MUX_S2, (channel >> 2) & 0x01);
    digitalWrite(MUX_S3, (channel >> 3) & 0x01);

    delay(1000);

    return true;
}

float MuxManager::readChannel(uint8_t channel)
{
    if(channel >= MUX_CHANNELS)
        return 0;

    selectChannel(channel);

    int adc = analogRead(MUX_SIG);

    float voltage =
        ((float)adc / 4095.0) * 3.3;

    float value = map(
        adc,
        0,
        4095,
        SENSOR_MIN[channel],
        SENSOR_MAX[channel]
    );

    sensorValues[channel] = value;

    Serial.print("[MUX] CH");
    Serial.print(channel);

    Serial.print(" ADC=");
    Serial.print(adc);

    Serial.print(" V=");
    Serial.print(voltage,3);

    Serial.print(" VALUE=");
    Serial.println(value);

    return value;
}

bool MuxManager::readAllChannels()
{
    Serial.println("[MUX] Reading Channels");

    for(uint8_t ch = 0; ch < MUX_CHANNELS; ch++)
    {
        readChannel(ch);
    }

    return true;
}

float MuxManager::getSensorValue(uint8_t channel)
{
    if(channel >= MUX_CHANNELS)
        return 0;

    return sensorValues[channel];
}


MuxData MuxManager::getAllSensorValues()
{
    MuxData data;

    for(uint8_t i = 0; i < MUX_CHANNELS; i++)
    {
        data.values[i] = sensorValues[i];
    }

    return data;
}