#include <Arduino.h>
#include "Config.h"
#include "LEDController.h"
#include "WiFiManager.h"
#include "FirebaseClient.h"
#include "DisplayController.h"

// Global instances
LEDController leds;
WiFiManager wifiManager;
FirebaseClient firebase;
DisplayController displayCtrl;

unsigned long lastCheckTime = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n=== E-ink Display System ===");
    
    // Initialize LEDs
    leds.init();
    leds.blueLEDOn();
    
    // Initialize display
    displayCtrl.init();
    displayCtrl.showMessage("Connecting to WiFi...");
    
    // Connect to WiFi
    if (!wifiManager.connect()) {
        Serial.println("Failed to connect to WiFi!");
        displayCtrl.showMessage("WiFi Failed!");
        leds.flashError();
        while (true) {
            delay(1000);
        }
    }
    
    leds.blueLEDOff();
    leds.pulseAmber();
    
    displayCtrl.showMessage("WiFi Connected!\nWaiting for images...");
    
    Serial.println("System ready!");
    Serial.printf("Checking Firebase every %lu seconds\n", CHECK_INTERVAL / 1000);
}

void loop() {
    unsigned long currentTime = millis();
    
    // Check Firebase at regular intervals
    if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
        lastCheckTime = currentTime;
        
        Serial.println("Checking for new images...");
        leds.blueLEDOn();
        
        String imageUrl;
        unsigned long timestamp;
        
        if (firebase.checkForNewImage(imageUrl, timestamp)) {
            Serial.println("New image found! Downloading...");
            leds.pulseAmber();
            
            uint8_t* imageBuffer = nullptr;
            size_t imageSize = 0;
            
            if (firebase.downloadImage(imageUrl, &imageBuffer, &imageSize)) {
                Serial.println("Image downloaded! Displaying...");
                displayCtrl.displayImage(imageBuffer, imageSize);
                
                // Flash both LEDs to indicate success
                leds.amberLEDOn();
                delay(500);
                leds.amberLEDOff();
                
                // Free the buffer
                free(imageBuffer);
                
                Serial.println("Image displayed successfully!");
            } else {
                Serial.println("Failed to download image");
                leds.flashError();
            }
        } else {
            Serial.println("No new images");
        }
        
        leds.blueLEDOff();
    }
    
    // Check WiFi connection
    if (!wifiManager.isConnected()) {
        Serial.println("WiFi disconnected! Reconnecting...");
        leds.flashError();
        displayCtrl.showMessage("WiFi Lost!\nReconnecting...");
        
        if (wifiManager.connect()) {
            displayCtrl.showMessage("WiFi Reconnected!");
            leds.pulseAmber();
        }
    }
    
    delay(100);
}