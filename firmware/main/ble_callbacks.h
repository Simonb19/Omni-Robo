#ifndef BLE_CALLBACKS_H
#define BLE_CALLBACKS_H

#include <BLEDevice.h>
#include <BLEServer.h>

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer);
  void onDisconnect(BLEServer* pServer);
};

class ControlCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic);
};

#endif
