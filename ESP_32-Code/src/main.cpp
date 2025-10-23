#include <Arduino.h>
#include "Config.h"
#include "LEDController.h"
#include "WiFiManager.h"
#include "FirebaseClient.h"
#include "MessageChecker.h"
#include "AppState.h"
#include "StorageManager.h"

LEDController ledController;
StorageManager storageManager;
AppState appState(storageManager);
WiFiManager wifiManager(ledController);
FirebaseClient firebaseClient;
MessageChecker messageChecker(firebaseClient, ledController, appState);

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n=== ESP32 E-ink Display ===");
    
    storageManager.init();
    appState.init();
    ledController.init();
    
    if (wifiManager.connect()) {
        messageChecker.check();
    }
}

void loop() {
    if (wifiManager.isConnected()) {
        messageChecker.check();
    }
    
    delay(Config::CHECK_INTERVAL_MS);
}