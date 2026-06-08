// ============================================================
//  Schnittstelle der Steuerlogik: JSON-Befehle verarbeiten,
//  Motoren/Servo/Stepper ansteuern und Status zurückmelden.
// ============================================================

#ifndef HANDLERS_H
#define HANDLERS_H

#include <Arduino.h>
#include <BLEDevice.h>
#include "types.h"

// Debug-Schalter: auf false setzen, um alle Debug-Ausgaben abzuschalten.
#define DEBUG_MODE true

// Debug-Makros: geben nur etwas aus, wenn DEBUG_MODE aktiv ist.
#define DEBUG_PRINT(x)   if(DEBUG_MODE) Serial.print(x)
#define DEBUG_PRINTLN(x) if(DEBUG_MODE) Serial.println(x)

// Globale Objekte, die in main.ino definiert sind.
extern RobotControls controls;
extern BLECharacteristic* pStatusCharacteristic;

void handleControlCommand(String jsonString); // JSON parsen + verteilen
void applyMotorControls();                     // Motorwerte auf Pins schreiben
void calculateMecanumMotors();                 // Fahrbefehl -> Motorwerte
void updateStatus();                           // Statusobjekt per BLE senden
void updateServo();                            // Greiferwinkel anpassen
void updateStepper();                          // Höhenachse schrittweise bewegen
void stopAllMotors();                          // Not-Aus / Failsafe

#endif
