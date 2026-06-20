#pragma once


//--------------------------------------------------
// Deployment Configuration
//--------------------------------------------------



//#define SENSOR_TOP
//#define SENSOR_MIDDLE
#define SENSOR_BOTTOM

//--------------------------------------------------
// Auto Location String
//--------------------------------------------------

#ifdef SENSOR_TOP

#define SENSOR_LOCATION "Top"

#elif defined(SENSOR_MIDDLE)

#define SENSOR_LOCATION "Middle"

#elif defined(SENSOR_BOTTOM)

#define SENSOR_LOCATION "Bottom"

#else

#define SENSOR_LOCATION "Middle"

#endif

#define ENV_SUIT_NUMBER 1



#define THINGSBOARD_SERVER "thingsboard.cloud"
#define THINGSBOARD_TOKEN  "XaskJY8sg6pnMaZpM0oJ"   // Middle Device Suit 1

#define SAMPLE_INTERVAL_MS 30000



#define STATUS_LED 2
#define SENSOR_RX 16
#define SENSOR_TX 17



// =============================
// Env Snsor Mux Pins
// =============================

#define MUX_EN     14

#define MUX_S0     25
#define MUX_S1     26
#define MUX_S2     27
#define MUX_S3     33

#define MUX_SIG    34

#define MUX_CHANNELS 16

// =============================
// Serial
// =============================


#define DEBUG_SERIAL Serial

#define LOG_INFO(x)  do{ DEBUG_SERIAL.print("[INFO] "); DEBUG_SERIAL.println(x);}while(0)
#define LOG_WARN(x)  do{ DEBUG_SERIAL.print("[WARN] "); DEBUG_SERIAL.println(x);}while(0)
#define LOG_ERROR(x) do{ DEBUG_SERIAL.print("[ERROR] "); DEBUG_SERIAL.println(x);}while(0)

