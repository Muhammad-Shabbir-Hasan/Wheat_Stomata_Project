#pragma once
#include <Arduino.h>
class StorageManager{
public:
bool begin();
bool hasData();
bool saveRecord(String record);
String getNextRecord();
bool deleteNextRecord();
bool clear();
bool isFull();
};
