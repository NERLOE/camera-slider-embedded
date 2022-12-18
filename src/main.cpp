#include <Arduino.h>
#include <Bluetooth.h>
#include <Settings.h>
#include <Slider.h>
#include <WiFi.h>
#include <Wire.h>
#include <math.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    preferences.begin("camslider", false);

    pinMode(ledPin, OUTPUT);
    pinMode(calibrationPin, INPUT_PULLUP);

    // Initialize Bluetooth Low Energy (BLE)
    Serial.println("Initializing bluetooth low energy...");
    initBLE();

    sliderController.init();
}

bool ledState = false;

void blinkLED() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
}

float easeInExpo(float x) {
    return x == 0 ? 0 : pow(2, 10 * x - 10);
}

void loop() {
    sliderController.run();

    int calibrationValue = digitalRead(calibrationPin);
    if (calibrationValue == LOW) {
        sliderController.stopCalibration();
    }
}