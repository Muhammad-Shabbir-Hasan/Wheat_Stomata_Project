#include <Arduino.h>
#include "MuxManager.h"

#include "Config.h"


// =============================
// Sensor Scaling
// =============================

// Example values.
// Change according to actual sensor ranges.
/*
const float SENSOR_MIN[MUX_CHANNELS] =
{
    
    0.0,     // CH3
    0.0     // CH3

};

const float SENSOR_MAX[MUX_CHANNELS] =
{
   
   
    100.0,   // H2
    100.0   // H2

};

*/



#ifdef SENSOR_TOP
const char* SENSOR_NAMES[MUX_CHANNELS] =
{
    "AQ",      // MQ135 (Air Quality)
    "H2",      // MQ8   (Hydrogen)
    "CH4",     // MQ4   (Methane)
    "CO",      // MQ7   (Carbon Monoxide)
    "LPG",     // MQ9   (Liquefied Petroleum Gas)
    "AirTemp",
    "AirHumidity",
    "env8",
    "env9",
    "env10",
    "env11",
    "env12",
    "env13",
    "env14",
    "env15",
    "env16"

};

#elif defined(SENSOR_MIDDLE)

const char* SENSOR_NAMES[MUX_CHANNELS] =
{
    "AQ",      // MQ135 (Air Quality)
    "H2",      // MQ8   (Hydrogen)
    "CH4",     // MQ4   (Methane)
    "CO",      // MQ7   (Carbon Monoxide)
    "LPG",     // MQ9   (Liquefied Petroleum Gas)
    "CO2",      // MH Z19 (Carbon Dioxide)  
    "O3",      // MQ131 (Ozone)
    "VOC",     // MQ138 (Volatile Organic Compounds)
    "NH3",     // MQ137 (Ammonia)
    "AirTemp",
    "AirHumidity",
    "env12",
    "env13",
    "env14",
    "env15",
    "env16"

};


#elif defined(SENSOR_BOTTOM)

const char* SENSOR_NAMES[MUX_CHANNELS] =
{
    "AQ",      // MQ135 (Air Quality)
    "H2",      // MQ8   (Hydrogen)
    "CH4",     // MQ4   (Methane)
    "CO",      // MQ7   (Carbon Monoxide)
    "LPG",     // MQ9   (Liquefied Petroleum Gas)
    "SM1",
    "SM2",
    "AirTemp",
    "AirHumidity",
    "SoilTemp",
    "SoilHumidity",
    "env12",
    "env13",
    "env14",
    "env15",
    "env16"

};

#else
const char* SENSOR_NAMES[MUX_CHANNELS] =
{
    "AQ",      // MQ135 (Air Quality)
    "H2",      // MQ8   (Hydrogen)
    "CH4",     // MQ4   (Methane)
    "CO",      // MQ7   (Carbon Monoxide)
    "LPG",     // MQ9   (Liquefied Petroleum Gas)
    "CO2",      // MH Z19 (Carbon Dioxide)  
    "O3",      // MQ131 (Ozone)
    "VOC",     // MQ138 (Volatile Organic Compounds)
    "NH3",     // MQ137 (Ammonia)
    "AirTemp",
    "AirHumidity",
    "env12",
    "env13",
    "env14",
    "env15",
    "env16"

};

#endif




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
    4095.0, // CO
    4095.0, // CO2
    4095.0,  // NH3
    4095.0,   // H2
    4095.0, // CO
    4095.0, // CO2
    4095.0,  // NH3
    4095.0,   // H2
    4095.0, // CO
    4095.0, // CO2
    4095.0,  // NH3
    4095.0,   // H2
    4095.0, // CO
    4095.0, // CO2
    4095.0,  // NH3
    4095.0   // H2
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

    int adc = analogRead(MUX_SIG); // waste 1st sample

    long sum = 0;

    for(int i = 0;
        i < NUM_SAMPLES;
        i++)
    {
        sum += analogRead(MUX_SIG);

        delay(SAMPLE_DELAY_MS);
    }

    adc = sum / NUM_SAMPLES;


    float voltage =
        ((float)adc / 4095.0) * 3.3;

    float value = map(
        adc,
        0,
        4095,
        SENSOR_MIN[channel],
        SENSOR_MAX[channel]
    );

    //sensorValues[channel] = value;
    sensorValues[channel] = adc;        //Loading raw data

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