#include <Arduino.h>
#include "Config.h"
#include "StatusLED.h"
enum Mode{OK,PROC,WARN,FATAL};
static Mode mode=PROC;
void initStatusLED(){pinMode(STATUS_LED,OUTPUT);}
void setLedOK(){mode=OK;}
void setLedProcessing(){mode=PROC;}
void setLedWarning(){mode=WARN;}
void setLedFatal(){mode=FATAL;}
void updateStatusLED(){}
