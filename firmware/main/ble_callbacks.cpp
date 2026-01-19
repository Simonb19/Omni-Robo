#include "ble_callbacks.h"
#include "handlers.h"
#include "constants.h"

void MyServerCallbacks::onConnect(BLEServer* pServer) {
  deviceConnected = true;
}

void MyServerCallbacks::onDisconnect(BLEServer* pServer) {
  deviceConnected = false;
  BLEDevice::startAdvertising();
}

void ControlCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
  String jsonString = String(pCharacteristic->getValue().c_str());
  DEBUG_PRINTLN("\n--- Received BLE Command ---");
  DEBUG_PRINT("Raw JSON: ");
  DEBUG_PRINTLN(jsonString);
  DEBUG_PRINT("Length: ");
  DEBUG_PRINTLN(jsonString.length());
  if (jsonString.length() > 0) {
    handleControlCommand(jsonString);
  }
}
