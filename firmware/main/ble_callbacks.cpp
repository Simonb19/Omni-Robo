// ============================================================
//  Umsetzung der BLE-Callbacks (siehe ble_callbacks.h).
// ============================================================

#include "ble_callbacks.h"
#include "handlers.h"
#include "constants.h"

// Verbindung aufgebaut: Flag setzen (loop() sendet ab jetzt Status).
void MyServerCallbacks::onConnect(BLEServer* pServer) {
  deviceConnected = true;
}

// Verbindung verloren: aus Sicherheit alles stoppen und wieder
// sichtbar werden (Advertising), damit eine Neuverbindung möglich ist.
void MyServerCallbacks::onDisconnect(BLEServer* pServer) {
  deviceConnected = false;
  stopAllMotors();           // Failsafe
  delay(500);
  BLEDevice::startAdvertising();
}

// Eingehender Steuerbefehl: JSON auslesen und an den Handler übergeben.
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
