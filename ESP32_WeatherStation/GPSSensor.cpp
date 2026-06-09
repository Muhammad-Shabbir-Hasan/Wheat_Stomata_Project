#include "GPSSensor.h"
bool GPSSensor::begin(){return true;}
bool GPSSensor::read(){return true;}
bool GPSSensor::hasFix(){return false;}
float GPSSensor::getLatitude(){return 0;}
float GPSSensor::getLongitude(){return 0;}
float GPSSensor::getAccuracy(){return 0;}
