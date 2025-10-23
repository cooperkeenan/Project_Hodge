#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {
    extern const char* WIFI_SSID;
    extern const char* WIFI_PASSWORD;
    extern const String FIREBASE_BUCKET;
    
    extern const int LED_BLUE;
    extern const int LED_AMBER;
    extern const int CHECK_INTERVAL_MS;
    extern const int LED_FLASH_COUNT;
    extern const int LED_FLASH_DELAY_MS;
}

#endif