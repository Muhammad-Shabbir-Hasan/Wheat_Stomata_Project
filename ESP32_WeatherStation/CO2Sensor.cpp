#include "CO2Sensor.h"

#include <Arduino.h>


//--------------------------------------------------
// Storage
//--------------------------------------------------

static int CO2_PPM = 0;

//--------------------------------------------------
// Begin
//--------------------------------------------------

bool CO2Sensor::begin()
{
    CO2_SERIAL.begin(
        9600,
        SERIAL_8N1,
        CO2_RX_PIN,
        CO2_TX_PIN);

    delay(100);

    return true;
}

//--------------------------------------------------
// Read Sensor
//--------------------------------------------------

bool CO2Sensor::read()
{
    const uint8_t EXPECTED_BYTES = 9;

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

        while(CO2_SERIAL.available())
        {
            CO2_SERIAL.read();
        }

        //--------------------------------------------------
        // Send Command
        //--------------------------------------------------

        CO2_SERIAL.write(
            CO2_REQUEST_FRAME,
            sizeof(CO2_REQUEST_FRAME));

        CO2_SERIAL.flush();

        //--------------------------------------------------
        // Wait For Response
        //--------------------------------------------------

        unsigned long startTime =
            millis();

        uint8_t count = 0;

        while((millis() - startTime)
                < 2000)
        {
            while(CO2_SERIAL.available())
            {
                if(count < EXPECTED_BYTES)
                {
                    response[count++] =
                        CO2_SERIAL.read();
                }
                else
                {
                    CO2_SERIAL.read();
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
                "[CO2] Invalid Length. Attempt ");

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
            ((uint16_t)response[2] << 8)
            |
            response[3];

        CO2_PPM =
            (float)value;

        Serial.print(
            "[CO2] PPM = ");

        Serial.println(
            CO2_PPM);

        return true;
    }

    //--------------------------------------------------
    // Failed All Attempts
    //--------------------------------------------------

    Serial.println(
        "[CO2] Read Failed");

    return false;
}

//--------------------------------------------------
// Get _PPM
//--------------------------------------------------

int CO2Sensor::getPPM()
{
    return CO2_PPM;
}