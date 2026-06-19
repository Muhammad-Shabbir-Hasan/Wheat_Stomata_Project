#include <WiFi.h>
#include "WiFiManagerCustom.h"
#include "StatusLED.h"

// Move these later to Config.h
//const char* WIFI_SSID     = "uofrGuest";
//const char* WIFI_PASSWORD = "";

const char* WIFI_SSID     = "iWorld";
const char* WIFI_PASSWORD = "123456789";


bool WiFiManagerCustom::begin()
{
    return true;
}

bool WiFiManagerCustom::connect()
{
    Serial.println("[WIFI] Connecting...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        updateStatusLED();

        if (millis() - startTime > 120000)
        {
            Serial.println("[WIFI] Connection timeout");

            setLedFatal();

            return false;
        }

        delay(100);
    }

    Serial.println("[WIFI] Connected");
    Serial.print("[WIFI] IP: ");
    Serial.println(WiFi.localIP());

    return updateTime();
}

bool WiFiManagerCustom::updateTime()
{
    Serial.println("[TIME] Synchronizing NTP");

    configTime(
        -6 * 3600,     // Saskatchewan UTC-6
        0,
        "pool.ntp.org",
        "time.nist.gov"
    );

    struct tm timeinfo;

    int retries = 0;

    while (!getLocalTime(&timeinfo))
    {
        delay(500);

        retries++;

        if (retries > 20)
        {
            Serial.println("[TIME] NTP Failed");

            setLedFatal();

            return false;
        }
    }

    timeValid = true;

    Serial.println("[TIME] Synchronization Complete");
    Serial.println(getDateTime());

    return true;
}

String WiFiManagerCustom::getDateTime()
{
    if (!timeValid)
    {
        return "TIME_INVALID";
    }

    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
    {
        return "TIME_INVALID";
    }

    char buffer[32];

    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M:%S",
        &timeinfo
    );

    return String(buffer);
}

bool WiFiManagerCustom::disconnect()
{
    WiFi.disconnect(true);

    return true;
}

bool WiFiManagerCustom::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}