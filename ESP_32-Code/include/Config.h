#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi Configuration
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// Firebase Configuration
extern const String FIREBASE_PROJECT;
extern const String FIREBASE_STORAGE_BUCKET;

// E-ink Display Pins
extern const int EPD_CS;
extern const int EPD_DC;
extern const int EPD_RST;
extern const int EPD_BUSY;
extern const int EPD_PWR;  // Power control for Rev 2.3

// SD Card Pin
extern const int SD_CS;

// LED Pins
extern const int BLUE_LED_PIN;
extern const int AMBER_LED_PIN;

// Timing
extern const unsigned long CHECK_INTERVAL;  // How often to check Firebase (ms)

#endif