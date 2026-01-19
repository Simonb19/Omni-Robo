#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "types.h"

// BLE UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CONTROL_CHAR_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define STATUS_CHAR_UUID    "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"

// LED timing
extern unsigned long previousMillis;
extern const long interval;
extern bool ledState;

// Heartbeat
extern unsigned long previousHeartbeatMillis;
extern const long heartbeatInterval;

// Servo
extern Servo gripperServo;
extern float currentServoAngle;
extern int currentGripperSpeed;
extern unsigned long previousServoMillis;
extern const long servoUpdateInterval;
extern const float minSpeed;
extern const float maxSpeed;

// Connection state
extern volatile bool deviceConnected;

// Pin definitions
extern const int LED_PIN;
extern const int SERVO_PIN;
extern const int AUX_MOTOR_PWM;
extern const int AUX_MOTOR_IN1;
extern const int AUX_MOTOR_IN2;
extern const int PWM_PIN[3];
extern const int IN_1_PIN[3];
extern const int IN_2_PIN[3];

// Motor angles
extern const float THETA_DEG[3];
extern const float ROTATION_SCALE;

// E_ALPHA declaration (defined in helpers.cpp)
extern Vector2D E_ALPHA[3];

// Initialize constants that need runtime setup
void initializeConstants();

#endif
