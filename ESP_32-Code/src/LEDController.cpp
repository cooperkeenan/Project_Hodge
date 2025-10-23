#include <Arduino.h>
#include "LEDController.h"
#include "Config.h"

void LEDController::init() {
    pinMode(Config::LED_BLUE, OUTPUT);
    pinMode(Config::LED_AMBER, OUTPUT);
    off();
}

void LEDController::off() {
    digitalWrite(Config::LED_BLUE, LOW);
    digitalWrite(Config::LED_AMBER, LOW);
}

void LEDController::flashNewMessage() {
    for (int i = 0; i < Config::LED_FLASH_COUNT; i++) {
        digitalWrite(Config::LED_AMBER, HIGH);
        delay(Config::LED_FLASH_DELAY_MS);
        digitalWrite(Config::LED_AMBER, LOW);
        delay(Config::LED_FLASH_DELAY_MS);
    }
}

void LEDController::blinkConnecting() {
    digitalWrite(Config::LED_BLUE, !digitalRead(Config::LED_BLUE));
}