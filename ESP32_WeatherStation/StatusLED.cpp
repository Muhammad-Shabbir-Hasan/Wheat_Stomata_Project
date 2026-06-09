#include <Arduino.h>
#include "Config.h"
#include "StatusLED.h"

static LedMode mode = LED_MODE_PROCESSING;

void initStatusLED()
{
    pinMode(STATUS_LED, OUTPUT);
}

void setLedOK()
{
    mode = LED_MODE_OK;
}

void setLedProcessing()
{
    mode = LED_MODE_PROCESSING;
}

void setLedWarning()
{
    mode = LED_MODE_WARNING;
}

void setLedFatal()
{
    mode = LED_MODE_FATAL;
}

void updateStatusLED()
{
}