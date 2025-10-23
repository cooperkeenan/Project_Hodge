#include "MessageChecker.h"

MessageChecker::MessageChecker(FirebaseClient& fb, LEDController& ledCtrl, AppState& appState) 
    : firebase(fb), led(ledCtrl), state(appState) {}

void MessageChecker::check() {
    Serial.println("Checking for new messages...");
    
    String messageId = firebase.getLatestMessageId();
    
    if (state.hasNewMessage(messageId)) {
        Serial.println("NEW MESSAGE: " + messageId);
        led.flashNewMessage();
    } else {
        Serial.println("No new messages");
    }
}