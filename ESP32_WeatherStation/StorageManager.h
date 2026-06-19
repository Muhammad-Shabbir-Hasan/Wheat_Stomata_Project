#pragma once

#include <Arduino.h>

class StorageManager
{
public:

    //--------------------------------------------------
    // Initialization
    //--------------------------------------------------

    bool begin();

    //--------------------------------------------------
    // Record Operations
    //--------------------------------------------------

    bool saveRecord(String record);

    bool hasData();

    String readAllData();

    String getNextRecord();

    bool deleteNextRecord();

    bool clear();

    //--------------------------------------------------
    // Storage Status
    //--------------------------------------------------

    bool isFull();

    uint32_t getUsedBytes();

    uint32_t getFreeBytes();

    uint32_t getTotalBytes();

    void printStorageInfo();

private:

    //--------------------------------------------------
    // Data File
    //--------------------------------------------------

    const char* dataFile = "/data.csv";
};