#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Bluetooth.h>
#include <Motor.h>
#include <WiFi.h>
#include <Wire.h>
#include <math.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    pinMode(ledPin, OUTPUT);

    // Initialize Bluetooth Low Energy (BLE)
    Serial.println("Initializing bluetooth low energy...");
    initBLE();

    // Initialize WIFI Access Point
    /*Serial.print("Setting AP (Access Point)…");
    WiFi.softAP("ESP32-CamSlider", "12345678", 1, 0, 1);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);*/
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
    if (motorSpeed != 0) {
        if ((motorPosition >= 100 && motorSpeed > 0) || (motorPosition <= 0 && motorSpeed < 0)) {
            return;
        }

        if (motorSpeed > 0) {
            motorPosition += 1;
        } else {
            motorPosition -= 1;
        }
    }

    if (deviceConnected) {
        char txString[8];
        dtostrf(motorPosition, 1, 2, txString);

        pTxCharacteristic->setValue(txString);

        pTxCharacteristic->notify();
    }

    if (motorSpeed != 0)
        delay(1100 - (abs(motorSpeed) * 10));  // bluetooth stack will go into congestion, if too many packets are sent
    else {
        delay(100);
    }
}