#include "GPSSensor.h"
#include <Arduino.h>


//--------------------------------------------------
// Begin
//--------------------------------------------------

bool GPSSensor::begin()
{
    GPS_SERIAL.begin(
        9600,
        SERIAL_8N1,
        GPS_RX_PIN,
        GPS_TX_PIN);

    delay(100);

    return true;
}

//--------------------------------------------------
// Read Latest GPS Data
//--------------------------------------------------

bool GPSSensor::read()
{
    latitude  = 0.0;
    longitude = 0.0;

    hdop = 0.0;
    vdop = 0.0;

    //--------------------------------------------------
    // Flush Old Data
    //--------------------------------------------------

    while(GPS_SERIAL.available())
    {
        GPS_SERIAL.read();
    }

    //--------------------------------------------------
    // Collect Fresh Data
    //--------------------------------------------------

    unsigned long startTime =
        millis();

    String latestGGA = "";
    String latestGSA = "";

    while((millis() - startTime) < 2000)
    {
        if(GPS_SERIAL.available())
        {
            String line =
                GPS_SERIAL.readStringUntil('\n');

            line.trim();
            Serial.println(line);

            if(line.startsWith("$GPGGA") ||
               line.startsWith("$GNGGA"))
            {
                latestGGA = line;
            }

            if(line.startsWith("$GPGSA") ||
               line.startsWith("$GNGSA"))
            {
                latestGSA = line;
            }
        }
    }

    //--------------------------------------------------
    // Parse GGA
    //--------------------------------------------------

    if(latestGGA.length() > 0)
    {
        Serial.println(
            latestGGA);

        String fields[20];

        uint8_t index = 0;
        int start = 0;

        for(int i = 0;
            i < latestGGA.length();
            i++)
        {
            if(latestGGA[i] == ',')
            {
                fields[index++] =
                    latestGGA.substring(
                        start,
                        i);

                start = i + 1;

                if(index >= 20)
                {
                    break;
                }
            }
        }

        fields[index] =
            latestGGA.substring(
                start);

        //--------------------------------------------------
        // Latitude
        //--------------------------------------------------

        if(fields[2].length() > 0)
        {
            double raw =
                fields[2].toDouble();

            int deg =
                (int)(raw / 100);

            double min =
                raw - (deg * 100);

            latitude =
                deg + (min / 60.0);

            if(fields[3] == "S")
            {
                latitude =
                    -latitude;
            }
        }

        //--------------------------------------------------
        // Longitude
        //--------------------------------------------------

        if(fields[4].length() > 0)
        {
            double raw =
                fields[4].toDouble();

            int deg =
                (int)(raw / 100);

            double min =
                raw - (deg * 100);

            longitude =
                deg + (min / 60.0);

            if(fields[5] == "W")
            {
                longitude =
                    -longitude;
            }
        }

        //--------------------------------------------------
        // HDOP
        //--------------------------------------------------

        if(fields[8].length() > 0)
        {
            hdop =
                fields[8].toFloat();
        }
    }

    //--------------------------------------------------
    // Parse GSA
    //--------------------------------------------------

    if(latestGSA.length() > 0)
    {
        Serial.println(
            latestGSA);

        String fields[25];

        uint8_t index = 0;
        int start = 0;

        for(int i = 0;
            i < latestGSA.length();
            i++)
        {
            if(latestGSA[i] == ',')
            {
                fields[index++] =
                    latestGSA.substring(
                        start,
                        i);

                start = i + 1;

                if(index >= 25)
                {
                    break;
                }
            }
        }

        fields[index] =
            latestGSA.substring(
                start);

        //--------------------------------------------------
        // VDOP
        //--------------------------------------------------

        if(fields[17].length() > 0)
        {
            vdop =
                fields[17].toFloat();
        }
    }

    //--------------------------------------------------
    // Validate
    //--------------------------------------------------

    if(latitude == 0.0 &&
       longitude == 0.0)
    {
        return false;
    }

    return true;
}

//--------------------------------------------------
// Getters
//--------------------------------------------------

double GPSSensor::getLat()
{
    return latitude * 10000000;
}

double GPSSensor::getLong()
{
    return longitude * 10000000;
}

float GPSSensor::getHDOP()
{
    return hdop * 1000;
}

float GPSSensor::getVDOP()
{
    return vdop * 1000;
}