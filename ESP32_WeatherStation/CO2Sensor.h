#pragma once
class CO2Sensor{
public:
bool begin();
bool read();
int getCO2ppm();
float getTemperature();
};
