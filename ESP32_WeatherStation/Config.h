#pragma once
#define STATUS_LED 2
#define SAMPLE_INTERVAL_MS 60000
#define SENSOR_RX 16
#define SENSOR_TX 17

#define DEBUG_SERIAL Serial

#define LOG_INFO(x)  do{ DEBUG_SERIAL.print("[INFO] "); DEBUG_SERIAL.println(x);}while(0)
#define LOG_WARN(x)  do{ DEBUG_SERIAL.print("[WARN] "); DEBUG_SERIAL.println(x);}while(0)
#define LOG_ERROR(x) do{ DEBUG_SERIAL.print("[ERROR] "); DEBUG_SERIAL.println(x);}while(0)
