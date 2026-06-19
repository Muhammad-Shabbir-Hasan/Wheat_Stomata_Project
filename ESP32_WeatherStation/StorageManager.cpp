#include "StorageManager.h"

#include <Arduino.h>
#include <LittleFS.h>

#define STORAGE_RESERVE_BYTES 10240   // 10 KB reserve

//--------------------------------------------------
// Initialize LittleFS
//--------------------------------------------------

bool StorageManager::begin()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("[Storage] LittleFS Mount Failed");
        return false;
    }

    Serial.println("[Storage] LittleFS Mounted");

    if (!LittleFS.exists(dataFile))
    {
        File file = LittleFS.open(
                        dataFile,
                        FILE_WRITE);

        if (!file)
        {
            Serial.println(
                "[Storage] Cannot Create Data File");

            return false;
        }

        file.close();
    }

    printStorageInfo();

    return true;
}

//--------------------------------------------------
// Save Record
//--------------------------------------------------

bool StorageManager::saveRecord(String record)
{
    uint32_t requiredBytes =
        record.length();

    if (requiredBytes > getFreeBytes())
    {
        Serial.println(
            "[Storage] Not Enough Flash Space");

        return false;
    }

    File file = LittleFS.open(
                    dataFile,
                    FILE_APPEND);

    if (!file)
    {
        Serial.println(
            "[Storage] File Open Failed");

        return false;
    }

    file.print(record);

    file.close();

    Serial.print(
        "[Storage] Record Saved : ");

    Serial.print(requiredBytes);

    Serial.println(" bytes");

    return true;
}

//--------------------------------------------------
// Check If Data Exists
//--------------------------------------------------

bool StorageManager::hasData()
{
    File file =
        LittleFS.open(
            dataFile,
            FILE_READ);

    if (!file)
    {
        return false;
    }

    bool result =
        (file.size() > 0);

    file.close();

    return result;
}

//--------------------------------------------------
// Read Entire File
//--------------------------------------------------

String StorageManager::readAllData()
{
    File file =
        LittleFS.open(
            dataFile,
            FILE_READ);

    if (!file)
    {
        return "";
    }

    String data =
        file.readString();

    file.close();

    return data;
}

//--------------------------------------------------
// Clear Storage
//--------------------------------------------------

bool StorageManager::clear()
{
    LittleFS.remove(dataFile);

    File file =
        LittleFS.open(
            dataFile,
            FILE_WRITE);

    if (!file)
    {
        return false;
    }

    file.close();

    Serial.println(
        "[Storage] Data Cleared");

    return true;
}

//--------------------------------------------------
// Storage Full Check
//--------------------------------------------------

bool StorageManager::isFull()
{
    return (
        getFreeBytes()
        < STORAGE_RESERVE_BYTES
    );
}

//--------------------------------------------------
// Storage Information
//--------------------------------------------------

uint32_t StorageManager::getUsedBytes()
{
    return LittleFS.usedBytes();
}

uint32_t StorageManager::getTotalBytes()
{
    return LittleFS.totalBytes();
}

uint32_t StorageManager::getFreeBytes()
{
    return
        LittleFS.totalBytes()
        - LittleFS.usedBytes();
}

//--------------------------------------------------
// Get First Record
//--------------------------------------------------

String StorageManager::getNextRecord()
{
    File file =
        LittleFS.open(
            dataFile,
            FILE_READ);

    if (!file)
    {
        return "";
    }

    String line =
        file.readStringUntil('\n');

    file.close();

    return line;
}

//--------------------------------------------------
// Placeholder
//--------------------------------------------------

bool StorageManager::deleteNextRecord()
{
    return true;
}

//--------------------------------------------------
// Print Statistics
//--------------------------------------------------

void StorageManager::printStorageInfo()
{
    Serial.println();

    Serial.println(
        "========== STORAGE ==========");

    Serial.print("Total Bytes : ");
    Serial.println(
        getTotalBytes());

    Serial.print("Used Bytes  : ");
    Serial.println(
        getUsedBytes());

    Serial.print("Free Bytes  : ");
    Serial.println(
        getFreeBytes());

    Serial.println(
        "=============================");
}