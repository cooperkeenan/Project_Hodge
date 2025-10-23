#include <WiFi.h>
#include "WiFiManager.h"
#include "Config.h"

WiFiManager::WiFiManager(LEDController& ledController) : led(ledController) {}

bool WiFiManager::connect() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(Config::WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(1000);
        Serial.print(".");
        led.blinkConnecting();
        attempts++;
    }
    
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Connected! IP: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    
    Serial.println("WiFi connection failed");
    return false;
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}