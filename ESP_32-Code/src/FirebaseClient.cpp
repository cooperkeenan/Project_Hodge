#include "FirebaseClient.h"
#include "Config.h"
#include <SD.h>

bool FirebaseClient::checkForNewImage(String& imageUrl, unsigned long long& timestamp) {
    HTTPClient http;
    
    String url = "https://firebasestorage.googleapis.com/v0/b/" + 
                 FIREBASE_STORAGE_BUCKET + "/o/messages%2Flatest.json?alt=media";
    
    Serial.printf("Checking: %s\n", url.c_str());
    
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode != 200) {
        Serial.printf("Failed to check Firebase: %d\n", httpCode);
        http.end();
        return false;
    }
    
    String payload = http.getString();
    http.end();
    
    Serial.printf("Response: %s\n", payload.c_str());
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
        Serial.println("Failed to parse Firebase response");
        return false;
    }
    
    String messageId = doc["messageId"].as<String>();
    unsigned long long newTimestamp = doc["timestamp"].as<unsigned long long>();
    
    Serial.printf("Current timestamp: %llu\n", newTimestamp);
    Serial.printf("Last checked: %llu\n", lastCheckedTimestamp);
    
    if (newTimestamp > lastCheckedTimestamp) {
        imageUrl = "https://firebasestorage.googleapis.com/v0/b/" + 
                   FIREBASE_STORAGE_BUCKET + "/o/images%2F" + 
                   messageId + ".png?alt=media";
        
        timestamp = newTimestamp;
        lastCheckedTimestamp = newTimestamp;
        
        Serial.printf("New image found! ID: %s\n", messageId.c_str());
        return true;
    }
    
    return false;
}

bool FirebaseClient::downloadImageToSD(const String& url, const String& filename) {
    // Deselect display before SD operations
    digitalWrite(EPD_CS, HIGH);
    
    HTTPClient http;
    http.begin(url);
    
    int httpCode = http.GET();
    
    if (httpCode != 200) {
        Serial.printf("Failed to download image: %d\n", httpCode);
        http.end();
        return false;
    }
    
    int len = http.getSize();
    if (len <= 0) {
        Serial.println("Invalid image size");
        http.end();
        return false;
    }
    
    Serial.printf("Image size: %d bytes\n", len);
    Serial.printf("Streaming to SD: %s\n", filename.c_str());
    
    // Open file on SD card
    File file = SD.open(filename.c_str(), FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing!");
        http.end();
        return false;
    }
    
    // Stream directly to SD card (no RAM needed!)
    WiFiClient* stream = http.getStreamPtr();
    uint8_t buffer[512];  // Small 512-byte buffer
    int downloaded = 0;
    
    while (http.connected() && downloaded < len) {
        size_t available = stream->available();
        if (available) {
            int toRead = min(available, sizeof(buffer));
            int read = stream->readBytes(buffer, toRead);
            file.write(buffer, read);
            downloaded += read;
            
            // Progress indicator every 10KB
            if (downloaded % 10240 == 0) {
                Serial.printf("  Downloaded: %d / %d bytes\n", downloaded, len);
            }
        }
        delay(1);
    }
    
    file.close();
    http.end();
    
    Serial.printf("✅ Downloaded %d bytes to SD card\n", downloaded);
    return downloaded == len;
}

// Keep old function for compatibility, but mark as deprecated
bool FirebaseClient::downloadImage(const String& url, uint8_t** buffer, size_t* size) {
    Serial.println("⚠️  Warning: downloadImage() tries to use RAM. Use downloadImageToSD() instead!");
    return false;
}