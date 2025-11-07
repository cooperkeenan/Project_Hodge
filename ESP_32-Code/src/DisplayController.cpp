#include "DisplayController.h"
#include "Config.h"
#include <Fonts/FreeMonoBold12pt7b.h>

void DisplayController::init() {
    // Initialize PWR pin
    pinMode(EPD_PWR, OUTPUT);
    digitalWrite(EPD_PWR, LOW);
    isPowered = false;
    
    // Create display instance
    display = new GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT>(
        GxEPD2_750_T7(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)
    );
    
    Serial.println("Display controller initialized");
}

void DisplayController::powerOn() {
    if (!isPowered) {
        // ⚠️ CRITICAL: Deselect SD card before using display!
        digitalWrite(SD_CS, HIGH);
        
        digitalWrite(EPD_PWR, HIGH);
        delay(100);  // Give display time to power up
        display->init(115200, true, 2, false);
        isPowered = true;
        Serial.println("Display powered ON");
    }
}

void DisplayController::powerOff() {
    if (isPowered) {
        display->hibernate();
        digitalWrite(EPD_PWR, LOW);
        isPowered = false;
        Serial.println("Display powered OFF");
    }
}

void DisplayController::showMessage(const char* message) {
    powerOn();
    
    // Deselect SD card during display operations
    digitalWrite(SD_CS, HIGH);
    
    display->setRotation(1);
    display->setFullWindow();
    
    // IMPORTANT: Do a full clear first to prevent ghosting/graininess
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
    } while (display->nextPage());
    
    delay(100);  // Brief pause between clear and draw
    
    // Now draw the message
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        display->setTextColor(GxEPD_BLACK);
        display->setFont(&FreeMonoBold12pt7b);
        display->setCursor(10, 30);
        display->println(message);
    } while (display->nextPage());
    
    Serial.printf("Displayed message: %s\n", message);
}

void DisplayController::clear() {
    powerOn();
    
    // Deselect SD card
    digitalWrite(SD_CS, HIGH);
    
    display->setFullWindow();
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
    } while (display->nextPage());
    
    Serial.println("Display cleared");
}

void DisplayController::displayImage(uint8_t* imageData, size_t size) {
    powerOn();
    
    // Deselect SD card
    digitalWrite(SD_CS, HIGH);
    
    display->setRotation(1);
    display->setFullWindow();
    
    // Clear first
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
    } while (display->nextPage());
    
    delay(100);
    
    // Show placeholder
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        display->setTextColor(GxEPD_BLACK);
        display->setFont(&FreeMonoBold12pt7b);
        display->setCursor(10, 30);
        display->println("New Image Received!");
        display->setCursor(10, 60);
        display->printf("Size: %d bytes", size);
        
        display->drawRect(50, 100, 700, 380, GxEPD_BLACK);
        display->setCursor(100, 300);
        display->println("Saved to SD Card");
        
    } while (display->nextPage());
    
    Serial.printf("Displayed image placeholder (%d bytes)\n", size);
}