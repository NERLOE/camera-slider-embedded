#pragma once

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Settings.h>

#define SERVICE_UUID "1f2d8a07-458d-44f0-a1b2-ecb92f5d3802"  // UART service UUID
#define CHARACTERISTIC_UUID_TX "7719d3cf-bb84-4bc3-a705-fa06e2ccd285"
#define CHARACTERISTIC_UUID_RX "593122f1-bc31-46ae-9521-2f86e2ea2740"

// BLE Server
extern BLEServer *pServer;
/// Notify Characteristic
extern BLECharacteristic *pTxCharacteristic;
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
            Serial.print("BLE RX: ");
            for (int i = 0; i < rxValue.length(); i++)
                Serial.print(rxValue[i]);
            Serial.println();

            // Do stuff based on the command received from BLE Central
            if (rxValue.find("1") != -1) {
                Serial.println("Turning LED on");
                digitalWrite(ledPin, HIGH);
            } else if (rxValue.find("0") != -1) {
                Serial.println("Turning LED off");
                digitalWrite(ledPin, LOW);
            }
        }
    }
};