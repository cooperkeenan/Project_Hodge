#include "LEDController.h"
#include "Config.h"

void LEDController::init() {
    pinMode(BLUE_LED_PIN, OUTPUT);
    pinMode(AMBER_LED_PIN, OUTPUT);
    digitalWrite(BLUE_LED_PIN, LOW);
    digitalWrite(AMBER_LED_PIN, LOW);
}

void LEDController::blueLEDOn() {
    digitalWrite(BLUE_LED_PIN, HIGH);
}

void LEDController::blueLEDOff() {
    digitalWrite(BLUE_LED_PIN, LOW);
}

void LEDController::amberLEDOn() {
    digitalWrite(AMBER_LED_PIN, HIGH);
}

void LEDController::amberLEDOff() {
    digitalWrite(AMBER_LED_PIN, LOW);
}

void LEDController::pulse(int pin) {
    for (int i = 0; i < 3; i++) {
        digitalWrite(pin, HIGH);
        delay(100);
        digitalWrite(pin, LOW);
        delay(100);
    }
}

void LEDController::pulseBlue() {
    pulse(BLUE_LED_PIN);
}

void LEDController::pulseAmber() {
    pulse(AMBER_LED_PIN);
}

void LEDController::flashError() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(BLUE_LED_PIN, HIGH);
        digitalWrite(AMBER_LED_PIN, HIGH);
        delay(200);
        digitalWrite(BLUE_LED_PIN, LOW);
        digitalWrite(AMBER_LED_PIN, LOW);
        delay(200);
    }
}