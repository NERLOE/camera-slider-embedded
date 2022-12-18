#include <Bluetooth.h>

BLEServer *pServer;
BLECharacteristic *sliderInfoCharacteristic;
bool deviceConnected = false;
bool bluetoothAdvertising = false;

void advertiseBLE() {
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);

    pServer->getAdvertising()->start();
    bluetoothAdvertising = true;
    Serial.println("BLE advertising started, waiting for client to connect...");
}

void initBLE(std::string deviceName) {
    // Create the BLE Device
    BLEDevice::init(deviceName);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create a BLE service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a Notifying BLE characteristic
    sliderInfoCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_SLIDER_INFO,
        BLECharacteristic::PROPERTY_NOTIFY);

    // BLE2902 needed to notify
    sliderInfoCharacteristic->addDescriptor(new BLE2902());

    // Create a BLE characteristic for receiving data
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    pRxCharacteristic->setCallbacks(new CharacteristicCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    advertiseBLE();
}