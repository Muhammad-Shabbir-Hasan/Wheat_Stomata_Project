#pragma once

enum LedMode
{
    LED_MODE_OK,
    LED_MODE_PROCESSING,
    LED_MODE_WARNING,
    LED_MODE_FATAL
};

void initStatusLED();
void updateStatusLED();

void setLedOK();
void setLedProcessing();
void setLedWarning();
void setLedFatal();