// ============================================================
//  Zentrale Datentypen: Eingaben vom Handy, Motorzustände und
//  der zusammengefasste Roboter-Zustand (RobotControls).
// ============================================================

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// ============================================================
//  Allgemeine Typen
// ============================================================

// 2D-Vektor, u. a. für die Radrichtungsvektoren (E_ALPHA).
struct Vector2D {
  float x;
  float y;
};

// Eindeutige ID je Rad (0,1,2) für die Kinematik-Berechnung.
typedef enum {
  WHEEL_1 = 0,
  WHEEL_2 = 1,
  WHEEL_3 = 2
} WheelID;

// ============================================================
//  Motor-Typen
// ============================================================

// Drehrichtung des Motors über die beiden H-Brücken-Pins.
enum MotorDirection {
  EN1,  // Richtung 1 (IN1 = HIGH)
  EN2   // Richtung 2 (IN2 = HIGH)
};

// Ein einzelner Antriebsmotor: Richtung + PWM-Wert (0..255).
struct Motor {
  MotorDirection direction;
  uint8_t pwm;
};

// ============================================================
//  Eingabe-Typen (kommen per BLE vom Handy)
// ============================================================

// Fahrbefehl. Wertebereich jeweils -100..100.
struct DriveInput {
  bool omniMode;   // true = omnidirektional, false = Pivot/Drehen
  int8_t x;        // seitlich (nur im Omni-Modus genutzt)
  int8_t y;        // vor/zurück
  int8_t rotation; // Drehen um die eigene Achse
};

// Greiferbefehl. Wertebereich jeweils -100..100.
struct Gripper {
  int8_t openClose;  // öffnen/schließen (Servo)
  int8_t z;          // Höhe auf/ab (Stepper)
};

// ============================================================
//  Gesamter Roboter-Zustand
// ============================================================

struct RobotControls {
  DriveInput drive;
  Gripper gripper;

  bool debugMode;          // true = manuelle Einzelmotor-Steuerung
  Motor motors[3];         // aktuelle Werte der 3 Antriebsmotoren

  unsigned long lastUpdate; // Zeitstempel des letzten Updates (millis)
};

// ============================================================
//  Standard-Initialisierung
// ============================================================

// Liefert einen RobotControls-Zustand mit allen Werten auf 0/aus.
inline RobotControls createDefaultControls() {
  RobotControls ctrl;
  ctrl.drive.omniMode = false;
  ctrl.drive.x = 0;
  ctrl.drive.y = 0;
  ctrl.drive.rotation = 0;
  ctrl.gripper.openClose = 0;
  ctrl.gripper.z = 0;
  ctrl.debugMode = false;

  for (int i = 0; i < 3; i++) {
    ctrl.motors[i].direction = EN1;
    ctrl.motors[i].pwm = 0;
  }

  ctrl.lastUpdate = 0;
  return ctrl;
}

#endif
