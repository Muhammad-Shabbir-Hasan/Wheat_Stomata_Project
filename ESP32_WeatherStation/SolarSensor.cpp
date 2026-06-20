#include "SolarSensor.h"

#include <Arduino.h>


//--------------------------------------------------
// Storage
//--------------------------------------------------

static float solarRadiation = 0.0f;

//--------------------------------------------------
// Begin
//--------------------------------------------------

bool SolarSensor::begin()
{
    SOLAR_SERIAL.begin(
        4800,
        SERIAL_8N1,
        SOLAR_RX_PIN,
        SOLAR_TX_PIN);

    delay(100);

    return true;
}

//--------------------------------------------------
// Read Sensor
//--------------------------------------------------

bool SolarSensor::read()
{
    const uint8_t EXPECTED_BYTES = 7;

    uint8_t response[EXPECTED_BYTES];

    //--------------------------------------------------
    // Try 5 Times
    //--------------------------------------------------

    for(uint8_t attempt = 0;
        attempt < 5;
        attempt++)
    {
        //--------------------------------------------------
        // Flush RX Buffer
        //--------------------------------------------------

        while(SOLAR_SERIAL.available())
        {
            SOLAR_SERIAL.read();
        }

        //--------------------------------------------------
        // Send Command
        //--------------------------------------------------

        SOLAR_SERIAL.write(
            SOLAR_RADIATION_REQUEST_FRAME,
            sizeof(SOLAR_RADIATION_REQUEST_FRAME));

        SOLAR_SERIAL.flush();

        //--------------------------------------------------
        // Wait For Response
        //--------------------------------------------------

        unsigned long startTime =
            millis();

        uint8_t count = 0;

        while((millis() - startTime)
                < 2000)
        {
            while(SOLAR_SERIAL.available())
            {
                if(count < EXPECTED_BYTES)
                {
                    response[count++] =
                        SOLAR_SERIAL.read();
                }
                else
                {
                    SOLAR_SERIAL.read();
                }
            }

            if(count >= EXPECTED_BYTES)
            {
                break;
            }
        }

        //--------------------------------------------------
        // Verify Length
        //--------------------------------------------------

        if(count != EXPECTED_BYTES)
        {
            Serial.print(
                "[Solar] Invalid Length. Attempt ");

            Serial.println(
                attempt + 1);

            continue;
        }

        //--------------------------------------------------
        // Extract Data
        //
        // Byte 5 = MSB
        // Byte 6 = LSB
        //--------------------------------------------------

        uint16_t value =
            ((uint16_t)response[4] << 8)
            |
            response[5];

        solarRadiation =
            (float)value;

        Serial.print(
            "[Solar] Radiation = ");

        Serial.println(
            solarRadiation);

        return true;
    }

    //--------------------------------------------------
    // Failed All Attempts
    //--------------------------------------------------

    Serial.println(
        "[Solar] Read Failed");

    return false;
}

//--------------------------------------------------
// Get Radiation
//--------------------------------------------------

float SolarSensor::getRadiation()
{
    return solarRadiation;
}