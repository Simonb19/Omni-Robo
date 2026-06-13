// ============================================================
//  Initialisiert Hardware + BLE-Server (setup) und führt im
//  Hauptloop die wiederkehrenden Aufgaben aus: Heartbeat,
//  Status-LED, Servo- und Stepper-Aktualisierung.
// ============================================================

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <math.h>

#include "types.h"
#include "constants.h"
#include "helpers.h"
#include "handlers.h"
#include "ble_callbacks.h"

// Zentraler Zustand des Roboters (Antrieb, Greifer, Motoren).
RobotControls controls = createDefaultControls();

// BLE-Objekte: Server + die beiden Characteristics (Steuern / Status).
BLEServer* pServer = NULL;
BLECharacteristic* pControlCharacteristic = NULL;
BLECharacteristic* pStatusCharacteristic = NULL;

// ------------------------------------------------------------
//  setup() – einmalig beim Start: Hardware und BLE einrichten.
// ------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // Richtungsvektoren der Räder berechnen (siehe helpers.cpp).
  initializeConstants();

  // Status-LED
  pinMode(LED_PIN, OUTPUT);

  // --- Endlagenschalter (Z-Achse oben / unten) ---
  pinMode(BOTTOM_Z_PIN, INPUT);    // Externer PULLUP in der Schaltung
  pinMode(TOP_Z_PIN,    INPUT);    // Externer PULLUP in der Schaltung

  // --- Externe Status-LEDs am Gehäuse ---
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  // --- Antriebsmotoren: je PWM-Pin + 2 Richtungspins (H-Brücke) ---
  for (int i = 0; i < 3; i++) {
    pinMode(PWM_PIN[i], OUTPUT);
    pinMode(IN_1_PIN[i], OUTPUT);
    pinMode(IN_2_PIN[i], OUTPUT);
    digitalWrite(IN_1_PIN[i], LOW);
    digitalWrite(IN_2_PIN[i], LOW);
    ledcAttach(PWM_PIN[i], 20000, 8);   // 20 kHz, 8 Bit (0..255), eigene Kanäle --> Start: alle Motoren aus
  }

  // --- Stepper (Höhenachse Z) ---
  pinMode(STEPPER_EN, OUTPUT);
  pinMode(STEPPER_STEP, OUTPUT);
  pinMode(STEPPER_DIR, OUTPUT);

  digitalWrite(STEPPER_EN, HIGH);   // HIGH = Treiber deaktiviert
  digitalWrite(STEPPER_STEP, LOW);
  digitalWrite(STEPPER_DIR, LOW);

  // --- Servo (Greifer öffnen/schließen) ---
  // Servo bekommt eigene LEDC-Timer, die die Motor-PWM NICHT anfasst
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  gripperServo.setPeriodHertz(50);            // Standard-Servo: 50 Hz
  gripperServo.attach(SERVO_PIN, 500, 2400);  // Pin, min/max Pulsweite in µs
  gripperServo.write((int)currentServoAngle); // Startwinkel anfahren

  // --- BLE einrichten ---
  BLEDevice::init("Omni Robo");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Control-Characteristic: Handy schreibt hier Steuerbefehle (JSON) hinein.
  pControlCharacteristic = pService->createCharacteristic(
    CONTROL_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pControlCharacteristic->setCallbacks(new ControlCallbacks());

  // Status-Characteristic: Roboter meldet seinen Zustand zurück (Notify).
  pStatusCharacteristic = pService->createCharacteristic(
    STATUS_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pStatusCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  // Advertising starten, damit der Roboter gefunden werden kann.
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE ready, waiting for connection...");
}

// ------------------------------------------------------------
//  loop() – läuft dauerhaft. Alle Aufgaben sind zeitgesteuert
//  (non-blocking über millis/micros), damit nichts blockiert.
// ------------------------------------------------------------
void loop() {
  unsigned long currentMillis = millis();

  // Heartbeat: alle 500 ms Status senden, solange verbunden.
  if (deviceConnected && (currentMillis - previousHeartbeatMillis >= heartbeatInterval)) {
    previousHeartbeatMillis = currentMillis;
    updateStatus();
  }

  // Status-LED + Gehäuse-LEDs.
  if (deviceConnected) {
    digitalWrite(LED_PIN, HIGH);
    setLedsConnected();          // Gehäuse-LEDs: dauerhaft an
  } else {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
    updateSearchAnimation();     // "Suchen": Lauflicht
  }

  // Servo schrittweise zum Zielwinkel bewegen.
  if (currentMillis - previousServoMillis >= servoUpdateInterval) {
    previousServoMillis = currentMillis;
    updateServo();
  }

  // Stepper-Schritte erzeugen (eigenes µs-Timing in updateStepper).
  updateStepper();
}
