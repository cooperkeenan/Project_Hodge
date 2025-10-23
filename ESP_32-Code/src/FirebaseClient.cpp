#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "FirebaseClient.h"
#include "Config.h"

String FirebaseClient::buildURL(const String& path) {
    return "https://firebasestorage.googleapis.com/v0/b/" + 
           Config::FIREBASE_BUCKET + "/o/" + path + "?alt=media";
}

String FirebaseClient::getLatestMessageId() {
    HTTPClient http;
    String url = buildURL("messages%2Flatest.json");
    
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode == 200) {
        String payload = http.getString();
        
        JsonDocument doc;
        deserializeJson(doc, payload);
        
        String messageId = doc["messageId"].as<String>();
        http.end();
        return messageId;
    }
    
    http.end();
    return "";
}