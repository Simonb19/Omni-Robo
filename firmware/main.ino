#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <math.h>

#include "types.h"
#include "constants.h"
#include "helpers.h"

// Debug flag - set to false to disable all debug output
#define DEBUG_MODE true

// Debug helper macro
#define DEBUG_PRINT(x) if(DEBUG_MODE) Serial.print(x)
#define DEBUG_PRINTLN(x) if(DEBUG_MODE) Serial.println(x)


// ============================================
// Global State
// ============================================
bool deviceConnected = false;
unsigned long previousMillis = 0;
unsigned long previousServoMillis = 0;
const long interval = 1000;
const long servoUpdateInterval = 20;
bool ledState = LOW;

Servo gripperServo;
int currentServoAngle = 90; 
int currentGripperSpeed = 0;

const float minSpeed = 1.0;
const float maxSpeed = 25.0;

RobotControls controls = createDefaultControls();

BLEServer* pServer = NULL;
BLECharacteristic* pControlCharacteristic = NULL;
BLECharacteristic* pStatusCharacteristic = NULL;

// ============================================
// Function Declarations
// ============================================
void handleControlCommand(String jsonString);
void updateStatus();
void updateServo();
void applyMotorControls();
void calculateMecanumMotors();

// ============================================
// BLE Callbacks
// ============================================
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    BLEDevice::startAdvertising();
  }
};

class ControlCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String jsonString = String(pCharacteristic->getValue().c_str());
    DEBUG_PRINTLN("\n--- Received BLE Command ---");
    DEBUG_PRINT("Raw JSON: ");
    DEBUG_PRINTLN(jsonString);
    DEBUG_PRINT("Length: ");
    DEBUG_PRINTLN(jsonString.length());
    if (jsonString.length() > 0) {
      handleControlCommand(jsonString);
    }
  }
};

// ============================================
// Control Handlers
// ============================================
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
      if (newValue != controls.gripper.openClose) {
        controls.gripper.openClose = newValue;
        currentGripperSpeed = newValue;
        updated = true;
      }
    }
    
    // TODO: Implement z-axis (height) control
    if (gripperObj.containsKey("z")) {
      controls.gripper.z = gripperObj["z"].as<int>();
      updated = true;
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
  // TODO: Implement your mecanum wheel calculations here
  // Use controls.drive.omniMode to determine which mode
  // Use controls.drive.x, y, rotation as inputs
  // Set controls.motor1/2/3 enable and direction
  
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
    if (currentServoAngle >= 130) currentServoAngle = 130;
  } else {
    currentServoAngle -= angleChange;
    if (currentServoAngle <= 40) currentServoAngle = 40;
  }
  
  gripperServo.write((int)currentServoAngle);
}

// ============================================
// Setup & Loop
// ============================================
void setup() {
  Serial.begin(115200);

  initializeConstants();
  
  // LED
  pinMode(LED_PIN, OUTPUT);

  // Setup LEDC channels for motor PWM
  for (int i = 0; i < 3; i++) {
    ledcAttach(PWM_PIN[i], 20000, 8);  // Pin, frequency (20kHz), resolution (8-bit)
  }

  
  // Motor pins
  for (int i = 0; i < 3; i++) {
    pinMode(IN_1_PIN[i], OUTPUT);
    pinMode(IN_2_PIN[i], OUTPUT);
  }
  
  // Servo
  gripperServo.attach(SERVO_PIN);
  gripperServo.write(currentServoAngle);

  // BLE Setup
  BLEDevice::init("Omni Robo");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pControlCharacteristic = pService->createCharacteristic(
    CONTROL_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pControlCharacteristic->setCallbacks(new ControlCallbacks());

  pStatusCharacteristic = pService->createCharacteristic(
    STATUS_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pStatusCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE ready, waiting for connection...");
}

void loop() {
  unsigned long currentMillis = millis();

  // LED status
  if (deviceConnected) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  }

  // Servo update
  if (currentMillis - previousServoMillis >= servoUpdateInterval) {
    previousServoMillis = currentMillis;
    updateServo();
  }
}
