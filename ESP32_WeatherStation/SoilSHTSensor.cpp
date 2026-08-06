#include "SoilSHTSensor.h"

#include "Config.h"

//--------------------------------------------------
// Begin
//--------------------------------------------------

bool SoilSHTSensor::begin()
{
    Wire.begin(
        I2C_SDA_PIN,
        I2C_SCL_PIN);

    //--------------------------------------------------
    // Initialize Sensor
    //--------------------------------------------------

    if(!sht31.begin(0x44))
    {
        Serial.println(
            "[SoilSHT] Initialization Failed");

        return false;
    }

    Serial.println(
        "[SoilSHT] Initialized");

    return true;
}

//--------------------------------------------------
// Read Sensor
//--------------------------------------------------

bool SoilSHTSensor::read()
{
    //--------------------------------------------------
    // Read Temperature
    //--------------------------------------------------

    soilTemperature =
        sht31.readTemperature();

    //--------------------------------------------------
    // Read Humidity
    //--------------------------------------------------

    soilHumidity =
        sht31.readHumidity();

    //--------------------------------------------------
    // Check Valid Data
    //--------------------------------------------------

    if(isnan(
            soilTemperature) ||
       isnan(
            soilHumidity))
    {
        Serial.println(
            "[SoilSHT] Read Failed");

        return false;
    }

    //--------------------------------------------------
    // Display
    //--------------------------------------------------

    Serial.print(
        "[SoilSHT] Temperature : ");

    Serial.print(
        soilTemperature);

    Serial.println(
        " C");

    Serial.print(
        "[SoilSHT] Humidity    : ");

    Serial.print(
        soilHumidity);

    Serial.println(
        " %");

    return true;
}

//--------------------------------------------------
// Get Temperature
//--------------------------------------------------

float SoilSHTSensor::getTemperature()
{
    return soilTemperature;
}

//--------------------------------------------------
// Get Humidity
//--------------------------------------------------

float SoilSHTSensor::getHumidity()
{
    return soilHumidity;
}