#pragma once
class GPSSensor{
public:
bool begin();
bool read();
bool hasFix();
float getLatitude();
float getLongitude();
float getAccuracy();
};
