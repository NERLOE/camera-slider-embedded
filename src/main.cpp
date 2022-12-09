#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Bluetooth.h>
#include <WiFi.h>
#include <Wire.h>
#include <math.h>

int motorPosition = 0;

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

bool flipped = false;
void loop() {
    if (motorPosition >= 100) {
        flipped = true;
    } else if (motorPosition <= 0) {
        flipped = false;
    }

    if (flipped) {
        motorPosition--;
    } else {
        motorPosition++;
    }

    float progress = motorPosition / 100.0;
    int delayTime = easeInExpo(progress) * 750 + 100;

    if (deviceConnected) {
        // int txValue = random(-100, 100);

        char txString[8];
        dtostrf(motorPosition, 1, 2, txString);

        pTxCharacteristic->setValue(txString);

        pTxCharacteristic->notify();
    }

    delay(delayTime);  // bluetooth stack will go into congestion, if too many packets are sent

    /**if (bluetoothAdvertising) {
        blinkLED();
        delay(1000);
    }*/
}