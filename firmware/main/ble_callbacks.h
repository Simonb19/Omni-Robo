// ============================================================
//  Callback-Klassen, die auf BLE-Ereignisse reagieren:
//  Verbinden/Trennen sowie eingehende Steuerbefehle.
// ============================================================

#ifndef BLE_CALLBACKS_H
#define BLE_CALLBACKS_H

#include <BLEDevice.h>
#include <BLEServer.h>

// Reagiert auf Auf- und Abbau der BLE-Verbindung.
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer);
  void onDisconnect(BLEServer* pServer);
};

// Wird aufgerufen, wenn das Handy in die Control-Characteristic schreibt.
class ControlCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic);
};

#endif
