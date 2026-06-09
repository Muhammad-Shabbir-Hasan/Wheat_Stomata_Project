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
    

    
    cloud.uploadPending();
    mux.readAllChannels();

    storage.saveRecord("{}");

    setLedOK();
}

void setup()
{
    initStatusLED();

    wifi.begin();
    storage.begin();
    cloud.begin();
    mux.begin();

    setLedProcessing();
    wifi.connect();
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
