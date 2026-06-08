// ============================================================
//  Deklaration aller globalen Konstanten und Zustandsvariablen.
//  Die tatsächlichen Werte werden in constants.cpp gesetzt
//  (extern = "existiert woanders, hier nur bekannt machen").
// ============================================================

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
#include "types.h"
#include <ESP32Servo.h>

// --- BLE-UUIDs (müssen mit dem Frontend übereinstimmen) ---
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CONTROL_CHAR_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define STATUS_CHAR_UUID    "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"

// --- Status-LED: Blink-Timing ---
extern unsigned long previousMillis;
extern const long interval;
extern bool ledState;

// --- Heartbeat: Intervall fürs Status-Senden ---
extern unsigned long previousHeartbeatMillis;
extern const long heartbeatInterval;

// --- Servo (Greifer) ---
extern Servo gripperServo;
extern float currentServoAngle;       // aktueller Winkel
extern int currentGripperSpeed;       // -100..100, steuert die Winkeländerung
extern unsigned long previousServoMillis;
extern const long servoUpdateInterval;
extern const float minSpeed;          // min. Winkeländerung pro Schritt
extern const float maxSpeed;          // max. Winkeländerung pro Schritt
extern const float SERVO_MIN_ANGLE;   // mechanischer Endanschlag unten
extern const float SERVO_MAX_ANGLE;   // mechanischer Endanschlag oben

// --- Verbindungsstatus (volatile: wird im BLE-Callback geändert) ---
extern volatile bool deviceConnected;

// --- Stepper (Höhenachse Z) ---
extern volatile int currentZSpeed;    // -100..100, Richtung + Geschwindigkeit
extern unsigned long stepperInterval;
extern unsigned long previousStepperMillis;
extern const unsigned long MIN_STEP_INTERVAL;  // kleinstes Intervall = schnellste Bewegung
extern const unsigned long MAX_STEP_INTERVAL;  // größtes Intervall  = langsamste Bewegung

// --- Pin-Belegung ---
extern const int LED_PIN;
extern const int SERVO_PIN;
extern const int STEPPER_EN;
extern const int STEPPER_STEP;
extern const int STEPPER_DIR;
extern const int BOTTOM_Z_PIN;        // unterer Endschalter (reserviert)
extern const int TOP_Z_PIN;           // oberer Endschalter (reserviert)
extern const int PWM_PIN[3];
extern const int IN_1_PIN[3];
extern const int IN_2_PIN[3];

// --- Kinematik ---
extern const float THETA_DEG[3];      // Einbauwinkel der 3 Räder in Grad
extern const float ROTATION_SCALE;    // Gewichtung des Drehanteils

// Richtungs-Einheitsvektoren je Rad (berechnet in helpers.cpp).
extern Vector2D E_ALPHA[3];

// Berechnet zur Laufzeit die Werte, die nicht konstant gesetzt werden können.
void initializeConstants();

#endif
