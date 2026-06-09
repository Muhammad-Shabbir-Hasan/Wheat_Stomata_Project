/*
void setup()
{
    Serial.begin(115200);

    Serial.println("Hello");
}

void loop()
{
}
*/


#include "Config.h"
#include "StatusLED.h"
#include "WiFiManagerCustom.h"
#include "StorageManager.h"
#include "CloudUploader.h"
#include "MuxManager.h"

unsigned long lastCycle = 0;

WiFiManagerCustom wifi;
StorageManager storage;
CloudUploader cloud;
MuxManager mux;
void runCycle()
{
    Serial.println("[INFO] Starting acquisition cycle");

    cloud.uploadPending();
    mux.readAllChannels();

    storage.saveRecord("{}");

    Serial.println("[INFO] Acquisition cycle complete");

    setLedOK();
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("====================================");
    Serial.println(" ESP32 Weather Station");
    Serial.println(" University of Regina");
    Serial.println(" System Starting...");
    Serial.println("====================================");

    initStatusLED();

    Serial.println("[INFO] Initializing WiFi");
    wifi.begin();

    Serial.println("[INFO] Initializing Storage");
    storage.begin();

    Serial.println("[INFO] Initializing Cloud");
    cloud.begin();

    Serial.println("[INFO] Initializing MUX");
    mux.begin();

    setLedProcessing();

    Serial.println("[INFO] Connecting WiFi");
    wifi.connect();

    Serial.println("[INFO] Setup Complete");
}

void loop()
{
    updateStatusLED();

    if(millis() - lastCycle >= SAMPLE_INTERVAL_MS)
    {
        lastCycle = millis();

        runCycle();
    }
}