#include "Config.h"

// WiFi Configuration - CHANGE THESE!
const char* WIFI_SSID = "Monkes";
const char* WIFI_PASSWORD = "2monkesinapod";

// Firebase Configuration - CHANGE THESE!
const String FIREBASE_PROJECT = "projecthodge-da803";
const String FIREBASE_STORAGE_BUCKET = "projecthodge-da803.appspot.com";

// E-ink Display Pins
const int EPD_CS = 5;
const int EPD_DC = 21;
const int EPD_RST = 22;
const int EPD_BUSY = 4;
const int EPD_PWR = 25;  // Power control for Rev 2.3

// LED Pins
const int BLUE_LED_PIN = 2;    // Built-in LED
const int AMBER_LED_PIN = 4;   // External LED (change if needed)

// Timing
const unsigned long CHECK_INTERVAL = 10000;  // Check every 10 seconds