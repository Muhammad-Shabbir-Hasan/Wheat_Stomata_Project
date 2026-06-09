#pragma once
#include <Arduino.h>
struct DataRecord{
float env0,env1,env2,env3;
float latitude,longitude,gpsAccuracy;
int co2ppm;
float solarRadiation;
String toJSON();
};
