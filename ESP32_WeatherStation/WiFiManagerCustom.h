#pragma once

#include <time.h>

class WiFiManagerCustom
{
public:

    bool begin();

    bool connect();

    bool disconnect();

    bool isConnected();

    bool updateTime();

    String getDateTime();

private:

    bool timeValid = false;
};