// ============================================================
//  Definition (= tatsächliche Werte) aller in constants.h
//  deklarierten globalen Konstanten und Variablen.
// ============================================================

#include "constants.h"

// --- Status-LED: Blink-Timing ---
unsigned long previousMillis = 0;
const long interval = 500;            // Blinkintervall in ms (wenn getrennt)
bool ledState = LOW;

// --- Heartbeat ---
unsigned long previousHeartbeatMillis = 0;
const long heartbeatInterval = 500;   // alle 500 ms Status senden

// --- Servo (Greifer) ---
Servo gripperServo;
float currentServoAngle = 85;         // Startwinkel
int currentGripperSpeed = 0;
unsigned long previousServoMillis = 0;
const long servoUpdateInterval = 20;  // Update-Takt in ms
const float minSpeed = 0.5;           // Winkelschritt bei langsamster Stufe
const float maxSpeed = 3.0;           // Winkelschritt bei schnellster Stufe
const float SERVO_MIN_ANGLE = 80.0;   // Endanschlag unten
const float SERVO_MAX_ANGLE = 150.0;  // Endanschlag oben

// --- Stepper (Höhenachse Z) ---
volatile int currentZSpeed = 0;
unsigned long stepperInterval = 0;
unsigned long previousStepperMillis = 0;
const unsigned long MIN_STEP_INTERVAL = 2000;    // schnellste Stufe (µs zwischen Schritten)
const unsigned long MAX_STEP_INTERVAL = 10000;   // langsamste Stufe (µs zwischen Schritten)

// --- Verbindungsstatus ---
volatile bool deviceConnected = false;

// --- Pin-Belegung ---
const int LED_PIN = 2;
const int LED_PINS[4] = {16,17,22,23};
const int SERVO_PIN = 13;

const int PWM_PIN[3]  = {26, 27, 25};   // PWM (Geschwindigkeit) je Motor
const int IN_1_PIN[3] = {14, 18, 33};   // Richtungspin 1 je Motor
const int IN_2_PIN[3] = {12, 19, 32};   // Richtungspin 2 je Motor

const int STEPPER_EN = 5;
const int STEPPER_STEP = 4;
const int STEPPER_DIR = 21;

const int BOTTOM_Z_PIN = 34;   // Endlagenschalter Unten
const int TOP_Z_PIN = 35;      // Endlagenschalter Oben

// --- Kinematik ---
// Einbauwinkel der 3 Omni-Räder (gleichmäßig, 120° versetzt).
const float THETA_DEG[3] = {180.0, 300.0, 60.0};
const float ROTATION_SCALE = 1.0;       // 1.0 = Drehung gleich stark wie Translation
