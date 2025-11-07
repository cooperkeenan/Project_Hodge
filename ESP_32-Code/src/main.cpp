#include <Arduino.h>
#include "Config.h"
#include "LEDController.h"
#include "WiFiManager.h"
#include "FirebaseClient.h"
#include "DisplayController.h"
#include <SD.h>
#include <SPI.h>

// Global instances
LEDController leds;
WiFiManager wifiManager;
FirebaseClient firebase;
DisplayController displayCtrl;

unsigned long lastCheckTime = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n=== E-ink Display System with SD Card ===");
    
    // CRITICAL: Setup CS pins BEFORE SPI initialization
    Serial.println("Step 1: Configuring pins...");
    pinMode(EPD_PWR, OUTPUT);
    digitalWrite(EPD_PWR, LOW);    // Display OFF initially
    
    pinMode(EPD_CS, OUTPUT);
    digitalWrite(EPD_CS, HIGH);    // Display deselected
    
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);     // SD deselected
    
    delay(100);
    Serial.println("✅ Pins configured");
    
    // Initialize LEDs
    leds.init();
    leds.blueLEDOn();
    
    // Initialize SPI
    Serial.println("Step 2: Initializing SPI...");
    SPI.begin(18, 19, 23);  // SCK, MISO, MOSI
    delay(100);
    Serial.println("✅ SPI initialized");
    
    // Initialize SD card
    Serial.println("Step 3: Initializing SD card...");
    digitalWrite(EPD_CS, HIGH);  // Ensure display deselected
    
    if (!SD.begin(SD_CS, SPI)) {
        Serial.println("❌ SD Card initialization FAILED!");
        leds.flashError();
        while (true) {
            delay(1000);
        }
    }
    Serial.println("✅ SD Card initialized");
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card: %llu MB\n", cardSize);
    
    // Create images directory
    if (!SD.exists("/images")) {
        SD.mkdir("/images");
        Serial.println("Created /images directory");
    }
    
    // Initialize display
    Serial.println("Step 4: Initializing display...");
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
    
    displayCtrl.showMessage("WiFi Connected!\nSD Card Ready!\nWaiting for images...");
    
    Serial.println("\n=== System Ready ===");
    Serial.printf("Checking Firebase every %lu seconds\n", CHECK_INTERVAL / 1000);
}

void loop() {
    unsigned long currentTime = millis();
    
    // Check Firebase at regular intervals
    if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
        lastCheckTime = currentTime;
        
        Serial.println("\nChecking for new images...");
        leds.blueLEDOn();
        
        String imageUrl;
        unsigned long long timestamp;
        
        if (firebase.checkForNewImage(imageUrl, timestamp)) {
            Serial.println("New image found! Downloading to SD card...");
            leds.pulseAmber();
            
            // Deselect display during SD operations
            digitalWrite(EPD_CS, HIGH);
            digitalWrite(EPD_PWR, LOW);
            
            // Create filename
            String filename = "/images/" + String((unsigned long)timestamp) + ".png";
            
            // Stream directly to SD card
            if (firebase.downloadImageToSD(imageUrl, filename)) {
                Serial.printf("✅ Image saved: %s\n", filename.c_str());
                
                // Show success on display
                char message[200];
                snprintf(message, sizeof(message), 
                    "New Drawing Received!\n\nSaved to:\n%s\n\nReady for display!", 
                    filename.c_str());
                
                displayCtrl.showMessage(message);
                
                // Flash LEDs
                leds.amberLEDOn();
                delay(500);
                leds.amberLEDOff();
                
                Serial.println("✅ Complete!");
            } else {
                Serial.println("❌ Failed to download/save");
                displayCtrl.showMessage("Download Failed!");
                leds.flashError();
            }
        } else {
            Serial.println("No new images");
        }
        
        leds.blueLEDOff();
    }
    
    // Check WiFi
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