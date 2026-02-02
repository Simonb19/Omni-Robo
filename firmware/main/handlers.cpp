#include "handlers.h"
#include "constants.h"
#include "helpers.h"
#include <ArduinoJson.h>

void handleControlCommand(String jsonString) {
  DEBUG_PRINTLN("\n[handleControlCommand] Starting parse...");
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.print("JSON error: ");
    Serial.println(error.c_str());
    return;
  }

  bool updated = false;

  // Handle gripper
  if (doc.containsKey("gripper")) {
    JsonObject gripperObj = doc["gripper"];
    
    if (gripperObj.containsKey("openClose")) {
      int newValue = gripperObj["openClose"].as<int>();
      DEBUG_PRINT("openClose value: ");  // Add this
      DEBUG_PRINTLN(newValue);
      if (newValue != controls.gripper.openClose) {
        controls.gripper.openClose = newValue;
        currentGripperSpeed = newValue;
        updated = true;
      }
    }
    
    // Z-axis (height) control via stepper
    if (gripperObj.containsKey("z")) {
      int newZ = gripperObj["z"].as<int>();
      if (newZ != controls.gripper.z) {
        controls.gripper.z = newZ;
        currentZSpeed = newZ;  // Set stepper speed (-100 to 100)
        updated = true;
      }
    }
  }

 // Handle debug mode (manual motor control)
  if (doc.containsKey("debug")) {
    DEBUG_PRINTLN("Entered debug block");
    controls.debugMode = true;
    JsonObject debug = doc["debug"];
    
    // Replace the three separate motor1/motor2/motor3 blocks with:
    const char* motorKeys[3] = {"motor1", "motor2", "motor3"};
    for (int i = 0; i < 3; i++) {
      if (debug.containsKey(motorKeys[i])) {
        DEBUG_PRINT("Found motor: ");  // Add this
        DEBUG_PRINTLN(motorKeys[i]);
        JsonObject motor = debug[motorKeys[i]];
        if (motor.containsKey("pwm")) {
          controls.motors[i].pwm = motor["pwm"].as<uint8_t>();
          DEBUG_PRINT("Set PWM: ");  // Add this
          DEBUG_PRINTLN(controls.motors[i].pwm);
          updated = true;
        }
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
    applyMotorControls();  // Apply debug motor values
  }
  // Handle normal drive mode (mecanum calculation)
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
    
    calculateMecanumMotors();  // Calculate motor values from drive input
  }

  if (updated) {
    controls.lastUpdate = millis();
    updateStatus();
  }
}

void applyMotorControls() {
  DEBUG_PRINTLN(">>> Inside applyMotorControls");
  for (int i = 0; i < 3; i++) {
    DEBUG_PRINT("Motor "); DEBUG_PRINT(i);
    DEBUG_PRINT(" | PWM="); DEBUG_PRINT(controls.motors[i].pwm);
    DEBUG_PRINT(" | DIR="); DEBUG_PRINT(controls.motors[i].direction);
    DEBUG_PRINT(" | PWM_PIN="); DEBUG_PRINT(PWM_PIN[i]);
    DEBUG_PRINT(" | IN1_PIN="); DEBUG_PRINT(IN_1_PIN[i]);
    DEBUG_PRINT(" | IN2_PIN="); DEBUG_PRINTLN(IN_2_PIN[i]);
    if (controls.motors[i].pwm > 0) {
      DEBUG_PRINTLN("  -> Setting motor ON");
      digitalWrite(IN_1_PIN[i], controls.motors[i].direction == EN1 ? HIGH : LOW);
      digitalWrite(IN_2_PIN[i], controls.motors[i].direction == EN1 ? LOW : HIGH);
      ledcWrite(PWM_PIN[i], controls.motors[i].pwm);
    } else {
      DEBUG_PRINTLN("  -> Setting motor OFF");
      ledcWrite(PWM_PIN[i], 0);
      digitalWrite(IN_1_PIN[i], LOW);
      digitalWrite(IN_2_PIN[i], LOW);
    }
  }
}


void calculateMecanumMotors() {
  
  getMotorCommands(controls.drive.x, controls.drive.y, controls.drive.rotation, controls.motors, controls.drive.omniMode);
  
  applyMotorControls();
}

void updateStatus() {
  if (pStatusCharacteristic && deviceConnected) {
    StaticJsonDocument<256> doc;
    JsonObject gripperStatus = doc.createNestedObject("gripper");
    gripperStatus["openClose"] = controls.gripper.openClose;
    gripperStatus["z"] = controls.gripper.z;  // TODO: report actual z position when implemented

    doc["servoAngle"] = currentServoAngle;
    doc["speed"] = currentGripperSpeed;
    
    // Add motor status
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

void updateServo() {
  if (currentGripperSpeed == 0) return;
  
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
  
  // SIMPLIFIED: Just write the angle directly!
  gripperServo.write((int)currentServoAngle);
}

void updateStepper() {
  static unsigned long lastStepMicros = 0;
  static unsigned long lastDebug = 0;
  
  // Motor aus wenn Speed = 0
  if (currentZSpeed == 0) {
    digitalWrite(STEPPER_EN, HIGH);
    return;
  }
  
  // Motor aktivieren + Richtung setzen
  digitalWrite(STEPPER_EN, LOW);
  digitalWrite(STEPPER_DIR, currentZSpeed > 0 ? HIGH : LOW);
  
  // Interval berechnen (schneller = kleineres Interval)
  int absSpeed = abs(currentZSpeed);
  unsigned long interval = map(absSpeed, 1, 100, MAX_STEP_INTERVAL, MIN_STEP_INTERVAL);
  
  // Step ausführen wenn genug Zeit vergangen
  unsigned long now = micros();
  if (now - lastStepMicros >= interval) {
    lastStepMicros = now;
    
    digitalWrite(STEPPER_STEP, HIGH);
    delayMicroseconds(5);
    digitalWrite(STEPPER_STEP, LOW);
  }
  
  // Debug alle 1 Sekunde
  if (millis() - lastDebug > 1000) {
    lastDebug = millis();
    Serial.print("Stepper: speed=");
    Serial.print(currentZSpeed);
    Serial.print(", interval=");
    Serial.print(interval);
    Serial.println("us");
  }
}


