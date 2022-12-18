#pragma once
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Motor.h>
#include <Settings.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "1f2d8a07-458d-44f0-a1b2-ecb92f5d3802"  // UART service UUID
#define CHARACTERISTIC_UUID_SLIDER_INFO "2da31897-6b1f-4a83-811a-28b234fdc232"
#define CHARACTERISTIC_UUID_RX "593122f1-bc31-46ae-9521-2f86e2ea2740"

// BLE Server
extern BLEServer *pServer;
/// Notify Characteristic
extern BLECharacteristic *sliderInfoCharacteristic;
extern bool bluetoothAdvertising;
extern bool deviceConnected;

void advertiseBLE();
void initBLE(std::string deviceName = BLE_DEVICE_NAME);

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        Serial.println("BLE Client connected");
        bluetoothAdvertising = false;
        deviceConnected = true;
        digitalWrite(ledPin, HIGH);
    }

    void onDisconnect(BLEServer *pServer) {
        Serial.println("BLE Client disconnected");
        deviceConnected = false;
        digitalWrite(ledPin, LOW);
        advertiseBLE();
    }
};

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            // parse float
            // parse to int
            int value = (int)(atof(rxValue.c_str()) * 100);

            Serial.println("Motor speed changed: " + String(value) + "%");
        }
    }
};