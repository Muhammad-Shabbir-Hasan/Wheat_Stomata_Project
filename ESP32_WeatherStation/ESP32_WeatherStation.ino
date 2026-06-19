#include "Config.h"
#include "StatusLED.h"
#include "WiFiManagerCustom.h"
#include "StorageManager.h"
#include "CloudUploader.h"
#include "MuxManager.h"

#include <ArduinoJson.h>

//--------------------------------------------------
// Timing
//--------------------------------------------------

unsigned long lastCycle = 0;

unsigned long lastUploadTime = 0;

const unsigned long
    UPLOAD_INTERVAL_MS = 60000;   // 1 minute

//--------------------------------------------------
// Upload State
//--------------------------------------------------

bool uploadRequired = false;

//--------------------------------------------------
// Global Objects
//--------------------------------------------------

WiFiManagerCustom wifi;

StorageManager storage;

CloudUploader cloud;

MuxManager mux;

//--------------------------------------------------
// Sensor Data
//--------------------------------------------------

MuxData env_data;
//--------------------------------------------------
// Initialization
//--------------------------------------------------
bool initializeSystem()
{
    //--------------------------------------------------
    // WiFi
    //--------------------------------------------------

    Serial.println(
        "[INFO] Initializing WiFi");

    if(!wifi.begin())
    {
        Serial.println(
            "[ERROR] WiFi Init Failed");

        return false;
    }

    Serial.println(
        "[INFO] Connecting WiFi");

    if(!wifi.connect())
    {
        Serial.println(
            "[ERROR] WiFi Connection Failed");

        return false;
    }

    //--------------------------------------------------
    // Time
    //--------------------------------------------------

    Serial.println(
        "[INFO] Synchronizing Time");

    if(!wifi.updateTime())
    {
        Serial.println(
            "[ERROR] Time Sync Failed");

        return false;
    }

    //--------------------------------------------------
    // Storage
    //--------------------------------------------------

    Serial.println(
        "[INFO] Initializing Storage");

    if(!storage.begin())
    {
        Serial.println(
            "[ERROR] Storage Init Failed");

        return false;
    }

    //--------------------------------------------------
    // ThingsBoard
    //--------------------------------------------------

    Serial.println(
        "[INFO] Initializing ThingsBoard");

    if(!cloud.begin())
    {
        Serial.println(
            "[ERROR] ThingsBoard Init Failed");

        return false;
    }

    cloud.setSuitNumber(
        ENV_SUIT_NUMBER);

    cloud.setPosition(
        SENSOR_LOCATION);

    Serial.print(
        "[INFO] Device Name : ");

    Serial.println(
        cloud.getDeviceName());

    //--------------------------------------------------
    // MUX
    //--------------------------------------------------

    Serial.println(
        "[INFO] Initializing MUX");

    if(!mux.begin())
    {
        Serial.println(
            "[ERROR] MUX Init Failed");

        return false;
    }

    //--------------------------------------------------
    // Complete
    //--------------------------------------------------

    Serial.println(
        "[INFO] System Initialization Complete");

    return true;
}




//--------------------------------------------------
// Boot Menu
//--------------------------------------------------

bool processStartupMenu()
{
    while(true)
    {
        Serial.println();
        Serial.println("====================================");
        Serial.println("STARTUP MENU");
        Serial.println("====================================");
        Serial.println("1 - Read Stored Data");
        Serial.println("2 - Erase Stored Data");
        Serial.println("3 - Normal Operation");
        Serial.println();
        Serial.println("Waiting 10 seconds...");
        Serial.println();

        unsigned long startTime =
            millis();

        while((millis() - startTime)
                < 10000)
        {
            if(Serial.available())
            {
                char cmd =
                    Serial.read();

                switch(cmd)
                {
                    //--------------------------------------------------
                    // Read Data
                    //--------------------------------------------------

                    case '1':
                    {
                        Serial.println();
                        Serial.println(
                            "[MENU] Stored Data");

                        Serial.println(
                            "===== DATA START =====");

                        Serial.print(
                            storage.readAllData());

                        Serial.println(
                            "===== DATA END =====");

                        break;
                    }

                    //--------------------------------------------------
                    // Erase Data
                    //--------------------------------------------------

                    case '2':
                    {
                        Serial.println();
                        Serial.println(
                            "[MENU] Erasing Storage");

                        storage.clear();

                        Serial.println(
                            "[MENU] Storage Erased");

                        break;
                    }

                    //--------------------------------------------------
                    // Normal Operation
                    //--------------------------------------------------

                    case '3':
                    {
                        Serial.println();
                        Serial.println(
                            "[MENU] Normal Operation");

                        return true;
                    }

                    //--------------------------------------------------
                    // Invalid
                    //--------------------------------------------------

                    default:
                    {
                        Serial.println();
                        Serial.println(
                            "[MENU] Invalid Option");

                        break;
                    }
                }

                delay(500);

                // Return to menu
                break;
            }

            delay(10);
        }

        //--------------------------------------------------
        // Timeout
        //--------------------------------------------------

        if((millis() - startTime)
                >= 10000)
        {
            Serial.println(
                "[MENU] Timeout -> Normal Operation");

            return true;
        }
    }
}


//--------------------------------------------------
// Upload Pending Data
//--------------------------------------------------
bool uploadPendingData()
{
    //--------------------------------------------------
    // Check Pending Data
    //--------------------------------------------------

    if(!storage.hasData())
    {
        Serial.println(
            "[INFO] No Pending Data");

        return true;
    }

    Serial.println(
        "[INFO] Pending Data Found");

    //--------------------------------------------------
    // Read Data
    //--------------------------------------------------

    String pendingData =
        storage.readAllData();

    Serial.print(
        "[INFO] Pending Data Size : ");

    Serial.println(
        pendingData.length());

    //--------------------------------------------------
    // Upload
    //--------------------------------------------------

    if(!cloud.upload(
            pendingData))
    {
        Serial.println(
            "[WARNING] Pending Upload Failed");

        Serial.println(
            "[WARNING] Data Retained In Storage");

        return false;
    }

    //--------------------------------------------------
    // Clear Storage
    //--------------------------------------------------

    storage.clear();

    Serial.println(
        "[INFO] Pending Data Uploaded");

    Serial.println(
        "[INFO] Storage Cleared");

    return true;
}


//--------------------------------------------------
// Acquisition Cycle
//--------------------------------------------------

void runCycle()
{
    Serial.println();
    Serial.println("====================================");
    Serial.println("[INFO] Starting Acquisition");
    Serial.println("====================================");

    //--------------------------------------------------
    // Time Update
    //--------------------------------------------------

    if(!wifi.updateTime())
    {
        Serial.println(
            "[WARNING] Time Update Failed");

        setLedWarning();
    }

    //--------------------------------------------------
    // Read Environmental Sensors
    //--------------------------------------------------

    if(!mux.readAllChannels())
    {
        Serial.println(
            "[WARNING] Sensor Read Failed");

        setLedWarning();
    }

    env_data =
        mux.getAllSensorValues();

    //--------------------------------------------------
    // Build ThingsBoard JSON
    //--------------------------------------------------

    String payload = "{";

    payload += "\"timestamp\":\"";
    payload += wifi.getDateTime();
    payload += "\"";

    payload += ",\"suit\":";
    payload += String(
        ENV_SUIT_NUMBER);

    payload += ",\"position\":\"";
    payload += SENSOR_LOCATION;
    payload += "\"";

    for(uint8_t i=0; i<MUX_CHANNELS; i++)
    {
        payload += ",\"env";
        payload += String(i);
        payload += "\":";
        payload += String(
            env_data.values[i],
            2);
    }

    payload += "}";

    //--------------------------------------------------
    // Display Payload
    //--------------------------------------------------

    Serial.println("[DATA]");
    Serial.println(payload);

    //--------------------------------------------------
    // Upload Current Telemetry
    //--------------------------------------------------

    if(cloud.upload(payload))
    {
        Serial.println(
            "[INFO] Telemetry Uploaded");

        //--------------------------------------------------
        // Upload Any Pending Storage
        //--------------------------------------------------

        if(storage.hasData())
        {
            Serial.println(
                "[INFO] Uploading Pending Records");

            uploadPendingData();
        }

        lastUploadTime =
            millis();
    }
    else
    {
        //--------------------------------------------------
        // MQTT Failed
        // Store Record Locally
        //--------------------------------------------------

        Serial.println(
            "[WARNING] Telemetry Upload Failed");

        Serial.println(
            "[INFO] Saving To Local Storage");

        String backupRecord =
            payload + "\n";

        if(!storage.saveRecord(
                backupRecord))
        {
            Serial.println(
                "[ERROR] Local Save Failed");

            setLedWarning();
        }
    }

    //--------------------------------------------------
    // Storage Info
    //--------------------------------------------------

    storage.printStorageInfo();

    //--------------------------------------------------
    // Safety Upload
    //--------------------------------------------------

    if(storage.hasData())
    {
        if((millis() - lastUploadTime)
                >= UPLOAD_INTERVAL_MS)
        {
            Serial.println(
                "[INFO] Retrying Pending Upload");

            uploadPendingData();

            lastUploadTime =
                millis();
        }
    }

    //--------------------------------------------------
    // Normal State
    //--------------------------------------------------

    setLedOK();

    Serial.println(
        "[INFO] Acquisition Complete");
}


//--------------------------------------------------
// Setup
//--------------------------------------------------
void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("====================================");
    Serial.println("ESP32 Weather Station");
    Serial.println("University of Regina");
    Serial.println("System Starting...");
    Serial.println("====================================");

    //--------------------------------------------------
    // LED
    //--------------------------------------------------

    initStatusLED();

    // Setup State = Slow Blink
    setLedProcessing();

    
    //--------------------------------------------------
    // Boot Menu
    //--------------------------------------------------
    processStartupMenu();
    
    
    //--------------------------------------------------
    // Initialize System
    //--------------------------------------------------

    if(!initializeSystem())
    {
        Serial.println();
        Serial.println(
            "[FATAL] Initialization Failed");

        setLedFatal();

        while(true)
        {
            updateStatusLED();
        }
    }

    //--------------------------------------------------
    // Time
    //--------------------------------------------------

    Serial.print(
        "[INFO] Current Time : ");

    Serial.println(
        wifi.getDateTime());

    //--------------------------------------------------
    // Storage
    //--------------------------------------------------

    storage.printStorageInfo();

    //--------------------------------------------------
    // ThingsBoard Info
    //--------------------------------------------------

    Serial.println();

    Serial.println(
        "[INFO] ThingsBoard Enabled");

    Serial.print(
        "[INFO] Device Name : ");

    Serial.println(
        cloud.getDeviceName());

    //--------------------------------------------------
    // Upload Pending Data
    //--------------------------------------------------

    if(!uploadPendingData())
    {
        Serial.println(
            "[WARNING] Pending Upload Failed");

        Serial.println(
            "[WARNING] Data Kept In Storage");
    }
    else
    {
        Serial.println(
            "[INFO] Pending Upload Complete");
    }

    //--------------------------------------------------
    // Ready
    //--------------------------------------------------

    Serial.println(
        "[INFO] Setup Complete");

    Serial.println(
        "[INFO] System Ready");

    // Normal State = Solid ON
    setLedOK();

    lastCycle = millis();
}
//--------------------------------------------------
// Loop
//--------------------------------------------------

void loop()
{
    updateStatusLED();

    if(millis() - lastCycle >= SAMPLE_INTERVAL_MS)
    {
        lastCycle = millis();

        runCycle();

        delay(30000);
    }
}