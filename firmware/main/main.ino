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
#include "handlers.h"
#include "ble_callbacks.h"

RobotControls controls = createDefaultControls();

BLEServer* pServer = NULL;
BLECharacteristic* pControlCharacteristic = NULL;
BLECharacteristic* pStatusCharacteristic = NULL;

void setup() {
  Serial.begin(115200);

  initializeConstants();
  
  // LED
  pinMode(LED_PIN, OUTPUT);

  
  ESP32PWM::allocateTimer(0); // Timer 0 für Servo reservieren

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
  gripperServo.setPeriodHertz(50);
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

  // Heartbeat - send status every 500ms when connected
  if (deviceConnected && (currentMillis - previousHeartbeatMillis >= heartbeatInterval)) {
    previousHeartbeatMillis = currentMillis;
    updateStatus();
  }

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
