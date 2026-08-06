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
#define THINGSBOARD_SERVER "thingsboard.cloud"
#define THINGSBOARD_TOKEN  "YocXd3o4oGtS7WFkHDDE"   // Middle Device Suit 1


#elif defined(SENSOR_MIDDLE)

#define SENSOR_LOCATION "Middle"
#define THINGSBOARD_SERVER "thingsboard.cloud"
#define THINGSBOARD_TOKEN  "XaskJY8sg6pnMaZpM0oJ"   // Middle Device Suit 1



#elif defined(SENSOR_BOTTOM)

#define SENSOR_LOCATION "Bottom"
#define THINGSBOARD_SERVER "thingsboard.cloud"
#define THINGSBOARD_TOKEN  "AyU8bmWwZpK04b0AYm9m"   // Middle Device Suit 1


#else



#define SENSOR_LOCATION "Middle"
#define THINGSBOARD_SERVER "thingsboard.cloud"
#define THINGSBOARD_TOKEN  "XaskJY8sg6pnMaZpM0oJ"   // Middle Device Suit 1



#endif

#define ENV_SUIT_NUMBER 1







#define SAMPLE_INTERVAL_MS 20000

#define STATUS_LED 2




// =============================
// Env Snsor Mux Pins
// =============================

#define MUX_CHANNELS 16   //number of mux channels
#define MUX_SIG    35
#define MUX_S3     32
#define MUX_S2     33
#define MUX_S1     25
#define MUX_S0     26
#define MUX_EN     27








//--------------------------------------------------
// UART
//--------------------------------------------------


// =============================
// Serial
// =============================


#define DEBUG_SERIAL Serial

#define LOG_INFO(x)  do{ DEBUG_SERIAL.print("[INFO] "); DEBUG_SERIAL.println(x);}while(0)
#define LOG_WARN(x)  do{ DEBUG_SERIAL.print("[WARN] "); DEBUG_SERIAL.println(x);}while(0)
#define LOG_ERROR(x) do{ DEBUG_SERIAL.print("[ERROR] "); DEBUG_SERIAL.println(x);}while(0)

