#pragma once
class SolarSensor{
public:
bool begin();
bool read();
float getRadiation();
};
