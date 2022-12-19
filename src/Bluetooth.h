#pragma once
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Motor.h>
#include <Settings.h>
#include <Slider.h>
#include <Utils.h>

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
            String value = rxValue.c_str();

            if (value.indexOf("CALIBRATE") != -1) {
                sliderController.startCalibration();
            } else if (value.indexOf("ANIMATE") != -1) {  // Start animation with a string like this "ANIMATE;(position),(duration),(stop time);1,0.5,0.5;2,0.5,0.5;...;...;"
                Serial.println("Received animation command: " + value);
                try {
                    // Send the timeline to the slider controller
                    std::vector<String> split = splitString(value, ";");

                    Timeline timeline;

                    for (int i = 1; i < split.size(); i++) {
                        Serial.println("Keyframe: " + split[i]);
                        std::vector<String> keyframe = splitString(split[i], ",");

                        if (keyframe.size() < 3) {
                            Serial.println("Invalid keyframe command: " + String(split[i]));
                            break;
                        }

                        Serial.println("Keyframe length: " + String(keyframe.size()));

                        Keyframe kf;
                        Serial.println("Keyframe position: " + keyframe.at(0));
                        kf.position = keyframe.at(0).toInt();
                        Serial.println(kf.position);
                        Serial.println("Keyframe duration: " + keyframe.at(1));
                        kf.duration = keyframe.at(1).toDouble();
                        Serial.println(kf.duration);
                        Serial.println("Keyframe stopTime: " + keyframe.at(2));
                        kf.stopTime = keyframe.at(2).toDouble();
                        Serial.println(kf.stopTime);

                        timeline.keyframes.push_back(kf);
                    }

                    sliderController.runTimeline(timeline);
                } catch (const std::exception &e) {
                    Serial.println("Error parsing animation command: " + String(e.what()));
                }
            }
        }
    }
};