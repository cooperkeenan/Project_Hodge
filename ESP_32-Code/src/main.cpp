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
    Serial.println();
    Serial.println("=== E-ink Display System with SD Card ===");

    // Step 1: Configure pins before SPI
    Serial.println("Step 1: Configuring pins...");
    pinMode(EPD_PWR, OUTPUT);
    digitalWrite(EPD_PWR, LOW);     // Display off initially

    pinMode(EPD_CS, OUTPUT);
    digitalWrite(EPD_CS, HIGH);     // Deselect EPD

    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);      // Deselect SD
    delay(100);
    Serial.println("Pins configured");

    // LEDs
    leds.init();
    leds.blueLEDOn();

    // Step 2: SPI
    Serial.println("Step 2: Initializing SPI...");
    SPI.begin(18, 19, 23);          // SCK, MISO, MOSI
    delay(100);
    Serial.println("SPI initialized");

    // Step 3: SD card
    Serial.println("Step 3: Initializing SD card...");
    digitalWrite(EPD_CS, HIGH);     // Ensure EPD is deselected during SD init
    if (!SD.begin(SD_CS, SPI)) {
        Serial.println("SD Card initialization FAILED");
        leds.flashError();
        while (true) {
            delay(1000);
        }
    }
    Serial.println("SD Card initialized");

    uint64_t cardSizeMB = SD.cardSize() / (1024ULL * 1024ULL);
    Serial.printf("SD Card: %llu MB\n", (unsigned long long)cardSizeMB);

    if (!SD.exists("/images")) {
        SD.mkdir("/images");
        Serial.println("Created /images directory");
    }

    // Step 4: Display
    Serial.println("Step 4: Initializing display...");
    displayCtrl.init();
    displayCtrl.showMessage("Connecting to WiFi...");

    // WiFi
    if (!wifiManager.connect()) {
        Serial.println("Failed to connect to WiFi");
        displayCtrl.showMessage("WiFi Failed!");
        leds.flashError();
        while (true) {
            delay(1000);
        }
    }

    leds.blueLEDOff();
    leds.pulseAmber();

    displayCtrl.showMessage("WiFi Connected!\nSD Card Ready!\nWaiting for images...");

    Serial.println();
    Serial.println("=== System Ready ===");
    Serial.printf("Checking Firebase every %lu seconds\n", CHECK_INTERVAL / 1000);
}

void loop() {
    const unsigned long now = millis();

    // Periodic Firebase check
    if (now - lastCheckTime >= CHECK_INTERVAL) {
        lastCheckTime = now;

        Serial.println();
        Serial.println("Checking for new images...");
        leds.blueLEDOn();

        String imageUrl;
        unsigned long long timestamp64 = 0;

        if (firebase.checkForNewImage(imageUrl, timestamp64)) {
            Serial.println("New image found. Downloading to SD card...");
            leds.pulseAmber();

            // Keep EPD deselected and powered down during SD write
            digitalWrite(EPD_CS, HIGH);
            displayCtrl.powerOff();

            // Use lower 32 bits if backend timestamp is 64-bit
            unsigned long ts32 = static_cast<unsigned long>(timestamp64 & 0xFFFFFFFFULL);
            String filename = "/images/" + String(ts32) + ".png";

            if (firebase.downloadImageToSD(imageUrl, filename)) {
                Serial.printf("Image saved: %s\n", filename.c_str());

                // Ensure SD is deselected before EPD operations
                digitalWrite(SD_CS, HIGH);
                digitalWrite(EPD_CS, HIGH);

                Serial.println("Calling displayCtrl.displayImageFromSD()...");
                displayCtrl.displayImageFromSD(filename);

                // Success indication
                leds.amberLEDOn();
                delay(400);
                leds.amberLEDOff();

                Serial.println("Image render attempt complete");
                Serial.println("----------------------------------------");
            } else {
                Serial.println("Failed to download/save image");
                displayCtrl.showMessage("Download Failed!");
                leds.flashError();
            }
        } else {
            Serial.println("No new images");
        }

        leds.blueLEDOff();
    }

    // WiFi watchdog
    if (!wifiManager.isConnected()) {
        Serial.println("WiFi disconnected. Reconnecting...");
        leds.flashError();
        displayCtrl.showMessage("WiFi Lost!\nReconnecting...");

        if (wifiManager.connect()) {
            displayCtrl.showMessage("WiFi Reconnected!");
            leds.pulseAmber();
        }
    }

    delay(100);
}
