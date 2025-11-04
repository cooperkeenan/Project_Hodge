#include "WiFiManager.h"
#include "Config.h"

bool WiFiManager::connect() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < MAX_RETRIES) {
        delay(500);
        Serial.print(".");
        retries++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        printStatus();
        return true;
    } else {
        Serial.println("\nWiFi Connection Failed!");
        return false;
    }
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::printStatus() {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
}