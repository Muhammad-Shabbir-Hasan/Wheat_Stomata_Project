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
        CO2_SERIAL.flush();

        CO2_SERIAL.write(
            CO2_REQUEST_FRAME,
            sizeof(CO2_REQUEST_FRAME));

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
            while(CO2_SERIAL.available())
            {
                uint8_t b =
                    CO2_SERIAL.read();

                //--------------------------------------------------
                // Synchronize To 0xFF
                //--------------------------------------------------

                if(count == 0)
                {
                    if(b != 0xFF)
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
                "[CO2] Invalid Length. Attempt ");

            Serial.println(
                attempt + 1);

            continue;
        }

        //--------------------------------------------------
        // Verify Header
        //--------------------------------------------------

        if(response[0] != 0xFF ||
           response[1] != 0x86)
        {
            Serial.print(
                "[CO2] Invalid Header. Attempt ");

            Serial.println(
                attempt + 1);

            continue;
        }

        //--------------------------------------------------
        // Extract CO2
        //
        // Byte2 = MSB
        // Byte3 = LSB
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