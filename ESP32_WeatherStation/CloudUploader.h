#pragma once
#include <Arduino.h>
class CloudUploader{
public:
bool begin();
bool upload(String payload);
bool uploadPending();
};
