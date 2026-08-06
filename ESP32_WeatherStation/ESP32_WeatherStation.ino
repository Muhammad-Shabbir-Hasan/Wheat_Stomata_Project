#include "Config.h"
#include "StatusLED.h"
#include "WiFiManagerCustom.h"
#include "StorageManager.h"
#include "CloudUploader.h"
#include "MuxManager.h"
#include "SolarSensor.h"
#include "CO2Sensor.h"
#include "GPSSensor.h"
#include "DHT11Sensor.h"
#include "SoilSHTSensor.h"
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



SolarSensor solar;
CO2Sensor CO2;
GPSSensor GPS;
DHTSensor dht;
SoilSHTSensor soilSHT;


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

        //return false;
    }

    Serial.println(
        "[INFO] Connecting WiFi");

    if(!wifi.connect())
    {
        Serial.println(
            "[ERROR] WiFi Connection Failed");

       // return false;
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

       // return false;
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

        //return false;
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

       // return false;
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

       // return false;
    }

    //--------------------------------------------------
    // DHT11 Sensor
    //--------------------------------------------------


    dht.begin();

    //--------------------------------------------------
    // Soil SHT Sensor
    //--------------------------------------------------
    #ifdef SENSOR_BOTTOM
        Serial.println(
            "[INFO] Initializing Soil SHT");

        if(!soilSHT.begin())
        {
            Serial.println(
                "[ERROR] Soil SHT Init Failed");

          //  return false;
        }
    
    #endif
    //--------------------------------------------------
    // Solar Sensor
    //--------------------------------------------------

    #ifdef SENSOR_TOP

        Serial.println(
            "[INFO] Initializing Solar Sensor");

        if(!solar.begin())
        {
            Serial.println(
                "[ERROR] Solar Sensor Init Failed");

          //  return false;
        }

    #endif


    #ifdef SENSOR_MIDDLE

        Serial.println(
            "[INFO] Initializing CO2 Sensor");

        if(!CO2.begin())
        {
            Serial.println(
                "[ERROR] CO2 Sensor Init Failed");

        //    return false;
        }

    #endif



    #ifdef SENSOR_BOTTOM

        Serial.println(
            "[INFO] Initializing GPS Sensor");

        if(!GPS.begin())
        {
            Serial.println(
                "[ERROR] GPS Sensor Init Failed");

        //    return false;
        }

    #endif



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
    if(!storage.hasData())
    {
        Serial.println(
            "[INFO] No Pending Data");

        return true;
    }

    Serial.println(
        "[INFO] Pending Data Found");

    String pendingData =
        storage.readAllData();

    //--------------------------------------------------
    // Build ThingsBoard History Batch
    //--------------------------------------------------

    const size_t MAX_BATCH_SIZE = 2500;

    DynamicJsonDocument batchDoc(
        8192);

    JsonArray batch =
        batchDoc.to<JsonArray>();

    uint64_t baseTs =
        (uint64_t)time(nullptr)
        * 1000ULL;

    uint32_t recordIndex = 0;

    int uploadedBatches = 0;

    int startPos = 0;

    while(startPos < pendingData.length())
    {
        int endPos =
            pendingData.indexOf(
                '\n',
                startPos);

        if(endPos < 0)
        {
            endPos =
                pendingData.length();
        }

        String line =
            pendingData.substring(
                startPos,
                endPos);

        line.trim();

        if(line.length() > 0)
        {
            //--------------------------------------------------
            // Parse Stored JSON
            //--------------------------------------------------

            DynamicJsonDocument recordDoc(
                1024);

            if(deserializeJson(
                    recordDoc,
                    line) == DeserializationError::Ok)
            {
                JsonObject item =
                    batch.add<JsonObject>();

                item["ts"] =
                    baseTs +
                    recordIndex;

                JsonObject values =
                    item.createNestedObject(
                        "values");

                for(JsonPair kv :
                        recordDoc.as<JsonObject>())
                {
                    if(strcmp(
                           kv.key().c_str(),
                           "timestamp")
                           == 0)
                    {
                        continue;
                    }

                    values[kv.key()] =
                        kv.value();
                }

                recordIndex++;

                //--------------------------------------------------
                // Check Batch Size
                //--------------------------------------------------

                String testPayload;

                serializeJson(
                    batchDoc,
                    testPayload);

                if(testPayload.length()
                        > MAX_BATCH_SIZE)
                {
                    batch.remove(
                        batch.size() - 1);

                    String uploadPayload;

                    serializeJson(
                        batchDoc,
                        uploadPayload);

                    Serial.print(
                        "[INFO] Uploading History Batche : ");

                    Serial.println(
                        uploadPayload.length());
                                            

                    if(!cloud.upload(
                            uploadPayload))
                    {
                        Serial.println(
                            "[WARNING] History Upload Failed");

                        return false;
                    }

                    uploadedBatches++;

                    batchDoc.clear();

                    batch =
                        batchDoc.to<JsonArray>();

                    JsonObject newItem =
                        batch.add<JsonObject>();

                    newItem["ts"] =
                        baseTs +
                        recordIndex;

                    JsonObject newValues =
                        newItem.createNestedObject(
                            "values");

                    for(JsonPair kv :
                            recordDoc.as<JsonObject>())
                    {
                        if(strcmp(
                               kv.key().c_str(),
                               "timestamp")
                               == 0)
                        {
                            continue;
                        }

                        newValues[kv.key()] =
                            kv.value();
                    }
                }
            }
        }

        startPos =
            endPos + 1;
    }

    //--------------------------------------------------
    // Final Batch
    //--------------------------------------------------

    if(batch.size() > 0)
    {
        String uploadPayload;

        serializeJson(
            batchDoc,
            uploadPayload);

        Serial.print(
            "[INFO] Uploading Final History Batch : ");

        Serial.println(
            uploadPayload.length());



        Serial.print(
            "[INFO] Uploading Final History ---------------Batch : ");

        Serial.println(
            uploadPayload);

        if(!cloud.upload(
                uploadPayload))
        {
            Serial.println(
                "[WARNING] Final History Upload Failed");

            return false;
        }

        uploadedBatches++;
    }

    //--------------------------------------------------
    // Success
    //--------------------------------------------------

    storage.clear();

    Serial.print(
        "[INFO] Uploaded History Batches : ");

    Serial.println(
        uploadedBatches);

    Serial.println(
        "[INFO] Pending Data Uploaded");

    Serial.println(
        "[INFO] Storage Cleared");

    return true;
}


/*
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

    Serial.println(
    "[INFO] Pending Data : ");

    Serial.println(
        pendingData);

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
*/

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
//while(1)
{
    if(!mux.readAllChannels())
    {
        Serial.println(
            "[WARNING] Sensor Read Failed");

        setLedWarning();
    }
}

    env_data =
        mux.getAllSensorValues();


  
    //while(1)
    {   
    if(dht.read())
        {
            float temperature =
                dht.getTemperature();

            float humidity =
                dht.getHumidity();

                

            #ifdef SENSOR_TOP
                env_data.values[6-1] = temperature;
                env_data.values[7-1] = humidity;
                

            #endif

            #ifdef SENSOR_MIDDLE
                env_data.values[10-1] = temperature;
                env_data.values[11-1] = humidity;
                

            #endif

            #ifdef SENSOR_BOTTOM
                env_data.values[8-1] = temperature;
                env_data.values[9-1] = humidity;
                

            #endif

            /*Serial.print(
                "[DHT11] Temperature : ");

            Serial.print(
                temperature);

            Serial.println(
                " °C");

            Serial.print(
                "[DHT11] Humidity    : ");

            Serial.print(
                humidity);

            Serial.println(
                " %");*/


        }
    
    } 





    
    #ifdef SENSOR_BOTTOM  
 //   while(1)
 //   { 
        float soilTemperature = 0.0;

        float soilHumidity = 0.0;

        if(!soilSHT.read())
        {
            Serial.println(
                "[WARNING] Soil SHT Read Failed");

            setLedWarning();
        }
        else
        {
            soilTemperature =
                soilSHT.getTemperature();

            soilHumidity =
                soilSHT.getHumidity();
        }

        
        env_data.values[10-1] = soilTemperature;
        env_data.values[11-1] = soilHumidity;


//  }
    #endif
    
   
    
    #ifdef SENSOR_TOP

        float solarRadiation = 0.0;
//    while(1)
    {
        if(!solar.read())
        {
            Serial.println(
                "[WARNING] Solar Sensor Read Failed");

            setLedWarning();
        }
        else
        {
            solarRadiation =
                solar.getRadiation();
        }
    }
    #endif
    
    

    #ifdef SENSOR_MIDDLE

 
        int CO2_PPM = 0;  

//    while(1)
    {

        if(!CO2.read())
        {
            Serial.println(
                "[WARNING] CO2 Sensor Read Failed");

            setLedWarning();
        }
        else
        {
            CO2_PPM =
                CO2.getPPM();
        }
    }

    #endif   

    
    #ifdef SENSOR_BOTTOM

 //   while(1)

        float Lat = 0.0;
        float Long = 0.0;
        float HDOP = 0.0;
        float VDOP = 0.0;
        
//   {       

        if(!GPS.read())
        {
            Serial.println(
                "[WARNING] GPS Sensor Read Failed");

            setLedWarning();
        }
        else
        {
            Lat =
                GPS.getLat();

            Long =
                GPS.getLong();

            HDOP =
                GPS.getHDOP();

            VDOP =
                GPS.getVDOP();
        
        
        }
//    }

    #endif      
    
    
    
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

    payload += ",\"sts\":\"";
    payload += wifi.getDateTime();
    payload += "\"";
    
    payload += ",\"position\":\"";
    payload += SENSOR_LOCATION;
    payload += "\"";

    #ifdef SENSOR_TOP
        payload += ",\"solar_radiation\":";
        payload += String(
            solarRadiation);

    #endif


    
    #ifdef SENSOR_MIDDLE
        payload += ",\"CO2_PPM\":";
        payload += String(
            CO2_PPM);

    #endif


    #ifdef SENSOR_BOTTOM
        //float Lat,Long, HDOP, VDOP;
        payload += ",\"Lat\":";
        payload += String(
            Lat);

        payload += ",\"Long\":";
        payload += String(
            Long);

        payload += ",\"HDOP\":";
        payload += String(
            HDOP);

        payload += ",\"VDOP\":";
        payload += String(
            VDOP);

    #endif

    for(uint8_t i=0; i<MUX_CHANNELS; i++)
    {
        payload += ",\"";
        payload += SENSOR_NAMES[i];
        payload += "\":";
                
        payload += String( env_data.values[i],1);
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

        delay(10000);
    }
}