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
        SOLAR_SERIAL.flush();
        //--------------------------------------------------
        // Send Command
        //--------------------------------------------------

        SOLAR_SERIAL.write(
            SOLAR_RADIATION_REQUEST_FRAME,
            sizeof(SOLAR_RADIATION_REQUEST_FRAME));

        delay(100);

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
                uint8_t b =
                    SOLAR_SERIAL.read();

                //--------------------------------------------------
                // Synchronize To 0x01
                //--------------------------------------------------

                if(count == 0)
                {
                    if(b != 0x01)
                    {
                        continue;
                    }
                }

                response[count++] = b;

                if(count >= EXPECTED_BYTES)
                {
                    break;
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
        // Verify Header
        //--------------------------------------------------

        if(response[0] != 0x01 ||
           response[1] != 0x03 ||
           response[2] != 0x02)
        {
            Serial.print(
                "[Solar] Invalid Header. Attempt ");

            Serial.println(
                attempt + 1);

            continue;
        }

        //--------------------------------------------------
        // Extract Data
        //
        // Byte3 = MSB
        // Byte4 = LSB
        //--------------------------------------------------

        uint16_t value =
            ((uint16_t)response[3] << 8)
            |
            response[4];

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