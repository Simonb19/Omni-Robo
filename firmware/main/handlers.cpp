// ============================================================
//  Kern der Steuerlogik: nimmt JSON-Befehle vom Handy entgegen,
//  aktualisiert den Roboter-Zustand und steuert die Aktoren
//  (Motoren, Servo, Stepper). Meldet außerdem den Status zurück.
// ============================================================

#include "handlers.h"
#include "constants.h"
#include "helpers.h"
#include <ArduinoJson.h>

// ------------------------------------------------------------
//  handleControlCommand()
//  Parst das empfangene JSON und verteilt es auf die drei
//  Modi: Greifer, Debug (Einzelmotor) oder normaler Fahrbetrieb.
// ------------------------------------------------------------
void handleControlCommand(String jsonString) {
  DEBUG_PRINTLN("\n[handleControlCommand] Starting parse...");

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  // Ungültiges JSON -> abbrechen.
  if (error) {
    Serial.print("JSON error: ");
    Serial.println(error.c_str());
    return;
  }

  bool updated = false;  // true, sobald sich ein Wert geändert hat

  // --- Greifer (Servo + Z-Achse) ---
  if (doc.containsKey("gripper")) {
    JsonObject gripperObj = doc["gripper"];

    // Öffnen/Schließen (Servo)
    if (gripperObj.containsKey("openClose")) {
      int newValue = gripperObj["openClose"].as<int>();
      DEBUG_PRINT("openClose value: ");
      DEBUG_PRINTLN(newValue);
      if (newValue != controls.gripper.openClose) {
        controls.gripper.openClose = newValue;
        currentGripperSpeed = newValue;
        updated = true;
      }
    }

    // Höhe Z (Stepper)
    if (gripperObj.containsKey("z")) {
      int newZ = gripperObj["z"].as<int>();
      if (newZ != controls.gripper.z) {
        controls.gripper.z = newZ;
        currentZSpeed = newZ;   // Stepper-Geschwindigkeit (-100..100)
        updated = true;
      }
    }
  }

  // --- Debug-Modus: jeden Motor einzeln direkt ansteuern ---
  if (doc.containsKey("debug")) {
    DEBUG_PRINTLN("Entered debug block");
    controls.debugMode = true;
    JsonObject debug = doc["debug"];

    const char* motorKeys[3] = {"motor1", "motor2", "motor3"};
    for (int i = 0; i < 3; i++) {
      if (debug.containsKey(motorKeys[i])) {
        DEBUG_PRINT("Found motor: ");
        DEBUG_PRINTLN(motorKeys[i]);
        JsonObject motor = debug[motorKeys[i]];

        // PWM (Geschwindigkeit)
        if (motor.containsKey("pwm")) {
          controls.motors[i].pwm = motor["pwm"].as<uint8_t>();
          DEBUG_PRINT("Set PWM: ");
          DEBUG_PRINTLN(controls.motors[i].pwm);
          updated = true;
        }

        // Richtung (EN1 / EN2)
        if (motor.containsKey("direction")) {
          const char* dir = motor["direction"].as<const char*>();
          controls.motors[i].direction = (strcmp(dir, "EN2") == 0) ? EN2 : EN1;
          DEBUG_PRINT("Direction: ");
          DEBUG_PRINTLN(controls.motors[i].direction);
          updated = true;
        }
      }
    }

    DEBUG_PRINTLN("Calling applyMotorControls");
    applyMotorControls();   // Debug-Werte sofort ausgeben
  }

  // --- Normaler Fahrbetrieb (Kinematik-Berechnung) ---
  else if (doc.containsKey("drive")) {
    controls.debugMode = false;
    JsonObject drive = doc["drive"];

    if (drive.containsKey("omniMode")) {
      controls.drive.omniMode = drive["omniMode"].as<bool>();
      updated = true;
    }
    if (drive.containsKey("x")) {
      controls.drive.x = drive["x"].as<int8_t>();
      updated = true;
    }
    if (drive.containsKey("y")) {
      controls.drive.y = drive["y"].as<int8_t>();
      updated = true;
    }
    if (drive.containsKey("rotation")) {
      controls.drive.rotation = drive["rotation"].as<int8_t>();
      updated = true;
    }

    calculateMecanumMotors();  // Aus dem Fahrbefehl die Motorwerte berechnen
  }

  // Bei jeder Änderung Zeitstempel setzen und Status zurücksenden.
  if (updated) {
    controls.lastUpdate = millis();
    updateStatus();
  }
}

// ------------------------------------------------------------
//  applyMotorControls()
//  Schreibt die aktuellen Motorwerte (Richtung + PWM) auf die
//  H-Brücken-Pins.
// ------------------------------------------------------------
void applyMotorControls() {
  for (int i = 0; i < 3; i++) {
    DEBUG_PRINT("Motor "); DEBUG_PRINT(i);
    DEBUG_PRINT(" | PWM="); DEBUG_PRINT(controls.motors[i].pwm);
    DEBUG_PRINT(" | DIR="); DEBUG_PRINTLN(controls.motors[i].direction);

    // Drehrichtung über die beiden Richtungspins setzen.
    if (controls.motors[i].direction == EN1) {
      digitalWrite(IN_1_PIN[i], HIGH);
      digitalWrite(IN_2_PIN[i], LOW);
    } else {
      digitalWrite(IN_1_PIN[i], LOW);
      digitalWrite(IN_2_PIN[i], HIGH);
    }

    // Geschwindigkeit über PWM ausgeben.
    ledcWrite(PWM_PIN[i], controls.motors[i].pwm);
  }
}

// ------------------------------------------------------------
//  calculateMecanumMotors()
//  Übergibt den Fahrbefehl an die Kinematik (helpers.cpp) und
//  gibt die berechneten Werte aus.
//  Hinweis: x/y werden hier bewusst getauscht und y negiert,
//  damit die Achsen vom Joystick zur Roboter-Geometrie passen.
// ------------------------------------------------------------
void calculateMecanumMotors() {
  getMotorCommands(
    -controls.drive.y,        // -> x in der Kinematik
    controls.drive.x,         // -> y in der Kinematik
    controls.drive.rotation,  // -> omega
    controls.motors,
    controls.drive.omniMode
  );

  applyMotorControls();
}

// ------------------------------------------------------------
//  updateStatus()
//  Baut ein JSON mit dem aktuellen Roboterzustand und sendet
//  es per BLE-Notify an das Handy.
// ------------------------------------------------------------
void updateStatus() {
  if (pStatusCharacteristic && deviceConnected) {
    StaticJsonDocument<256> doc;

    // Greifer-Status
    JsonObject gripperStatus = doc.createNestedObject("gripper");
    gripperStatus["openClose"] = controls.gripper.openClose;
    gripperStatus["z"] = controls.gripper.z;

    doc["servoAngle"] = currentServoAngle;
    doc["speed"] = currentGripperSpeed;

    // Status aller 3 Antriebsmotoren
    const char* motorKeys[3] = {"motor1", "motor2", "motor3"};
    for (int i = 0; i < 3; i++) {
      JsonObject motorStatus = doc.createNestedObject(motorKeys[i]);
      motorStatus["pwm"] = controls.motors[i].pwm;
      motorStatus["direction"] = controls.motors[i].direction == EN1 ? "EN1" : "EN2";
    }

    String response;
    serializeJson(doc, response);

    pStatusCharacteristic->setValue(response.c_str());
    pStatusCharacteristic->notify();
  }
}

// ------------------------------------------------------------
//  updateServo()
//  Bewegt den Greifer-Servo schrittweise. Die Schrittweite
//  hängt von currentGripperSpeed ab; Endanschläge werden
//  begrenzt. Wird zeitgesteuert aus loop() aufgerufen.
// ------------------------------------------------------------
void updateServo() {
  if (currentGripperSpeed == 0) return;  // keine Bewegung gewünscht

  // Geschwindigkeit (0..100 %) auf einen Winkelschritt abbilden.
  float speedPercent = abs(currentGripperSpeed) / 100.0;
  float angleChange = minSpeed + (speedPercent * (maxSpeed - minSpeed));

  if (currentGripperSpeed > 0) {
    currentServoAngle += angleChange;
    if (currentServoAngle >= SERVO_MAX_ANGLE) currentServoAngle = SERVO_MAX_ANGLE;
  } else {
    currentServoAngle -= angleChange;
    if (currentServoAngle <= SERVO_MIN_ANGLE) currentServoAngle = SERVO_MIN_ANGLE;
  }

  DEBUG_PRINT("Servo written to: ");
  DEBUG_PRINTLN(currentServoAngle);

  gripperServo.write((int)currentServoAngle);
}

// ------------------------------------------------------------
//  updateStepper()
//  Erzeugt die Schrittimpulse für die Höhenachse. Geschwindigkeit
//  und Richtung kommen aus currentZSpeed (-100..100). Das Timing
//  läuft non-blocking über micros().
// ------------------------------------------------------------
void updateStepper() {
  static unsigned long lastStepMicros = 0;
  static unsigned long lastDebug = 0;

   // Endlagen lesen (LOW = Schalter ausgelöst)
   bool atBottom = (digitalRead(BOTTOM_Z_PIN) == LOW);
   bool atTop    = (digitalRead(TOP_Z_PIN)    == LOW);

   // Richtung blockieren, wenn Endlage erreicht
   // currentZSpeed > 0 = nach oben, < 0 = nach unten
   if (atTop    && currentZSpeed > 0) currentZSpeed = 0;
   if (atBottom && currentZSpeed < 0) currentZSpeed = 0;

  // Stillstand: Treiber deaktivieren und raus.
  if (currentZSpeed == 0) {
    digitalWrite(STEPPER_EN, HIGH);
    return;
  }

  // Treiber aktivieren + Richtung anhand des Vorzeichens setzen.
  digitalWrite(STEPPER_EN, LOW);
  digitalWrite(STEPPER_DIR, currentZSpeed > 0 ? HIGH : LOW);

  // Geschwindigkeit (1..100) auf das Schrittintervall abbilden
  // (höhere Geschwindigkeit = kleineres Intervall).
  int absSpeed = abs(currentZSpeed);
  unsigned long interval = map(absSpeed, 1, 100, MAX_STEP_INTERVAL, MIN_STEP_INTERVAL);

  // Einen Schritt ausführen, wenn genug Zeit vergangen ist.
  unsigned long now = micros();
  if (now - lastStepMicros >= interval) {
    lastStepMicros = now;

    digitalWrite(STEPPER_STEP, HIGH);
    delayMicroseconds(5);
    digitalWrite(STEPPER_STEP, LOW);
  }

  // Debug-Ausgabe einmal pro Sekunde.
  if (millis() - lastDebug > 1000) {
    lastDebug = millis();
    Serial.print("Stepper: speed=");
    Serial.print(currentZSpeed);
    Serial.print(", interval=");
    Serial.print(interval);
    Serial.println("us");
  }
}

// ------------------------------------------------------------
//  stopAllMotors()
//  Failsafe / Not-Aus: setzt alle Aktoren auf 0 und deaktiviert
//  den Stepper. Wird u. a. beim Verbindungsverlust aufgerufen.
// ------------------------------------------------------------
void stopAllMotors() {
  DEBUG_PRINTLN("!!! EMERGENCY STOP - All motors halted !!!");

  // Alle Antriebsmotoren stoppen.
  for (int i = 0; i < 3; i++) {
    controls.motors[i].pwm = 0;
    controls.motors[i].direction = EN1;

    digitalWrite(IN_1_PIN[i], LOW);
    digitalWrite(IN_2_PIN[i], LOW);
    ledcWrite(PWM_PIN[i], 0);
  }

  // Fahrbefehl zurücksetzen.
  controls.drive.x = 0;
  controls.drive.y = 0;
  controls.drive.rotation = 0;

  // Greifer und Stepper stoppen.
  currentGripperSpeed = 0;
  currentZSpeed = 0;
  digitalWrite(STEPPER_EN, HIGH);  // Stepper deaktivieren

  Serial.println("All motors stopped - Failsafe activated");
}
