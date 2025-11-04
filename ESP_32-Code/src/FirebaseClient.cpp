#include "FirebaseClient.h"
#include "Config.h"

bool FirebaseClient::checkForNewImage(String& imageUrl, unsigned long& timestamp) {
    HTTPClient http;
    
    // Access the latest.json file directly from Storage
    // iOS app uploads this file with messageId and timestamp
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
    
    // Parse JSON response
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
        Serial.println("Failed to parse Firebase response");
        return false;
    }
    
    // Get messageId and timestamp
    String messageId = doc["messageId"].as<String>();
    unsigned long newTimestamp = doc["timestamp"].as<unsigned long>();
    
    // Check if this is newer than what we've seen
    if (newTimestamp > lastCheckedTimestamp) {
        // Build the image URL
        // Format: messages/{messageId}.png
        imageUrl = "https://firebasestorage.googleapis.com/v0/b/" + 
                   FIREBASE_STORAGE_BUCKET + "/o/messages%2F" + 
                   messageId + ".png?alt=media";
        
        timestamp = newTimestamp;
        lastCheckedTimestamp = newTimestamp;
        
        Serial.printf("New image found! ID: %s\n", messageId.c_str());
        return true;
    }
    
    return false;
}

bool FirebaseClient::downloadImage(const String& url, uint8_t** buffer, size_t* size) {
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
    
    // Allocate buffer
    *buffer = (uint8_t*)malloc(len);
    if (*buffer == nullptr) {
        Serial.println("Failed to allocate memory for image");
        http.end();
        return false;
    }
    
    // Download to buffer
    WiFiClient* stream = http.getStreamPtr();
    int downloaded = 0;
    
    while (http.connected() && downloaded < len) {
        size_t available = stream->available();
        if (available) {
            int read = stream->readBytes(*buffer + downloaded, available);
            downloaded += read;
        }
        delay(1);
    }
    
    *size = downloaded;
    http.end();
    
    Serial.printf("Downloaded %d bytes\n", downloaded);
    return downloaded == len;
}