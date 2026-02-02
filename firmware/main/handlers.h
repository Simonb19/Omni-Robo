#ifndef HANDLERS_H
#define HANDLERS_H

#include <Arduino.h>
#include <BLEDevice.h>
#include "types.h"

// Debug flag - set to false to disable all debug output
#define DEBUG_MODE true

// Debug helper macro
#define DEBUG_PRINT(x) if(DEBUG_MODE) Serial.print(x)
#define DEBUG_PRINTLN(x) if(DEBUG_MODE) Serial.println(x)

// External references to global variables defined in main.ino
extern RobotControls controls;
extern BLECharacteristic* pStatusCharacteristic;

void handleControlCommand(String jsonString);
void applyMotorControls();
void calculateMecanumMotors();
void updateStatus();
void updateServo();
void updateStepper();

#endif
