#include "constants.h"

// LED timing
unsigned long previousMillis = 0;
const long interval = 500;
bool ledState = LOW;

// Heartbeat
unsigned long previousHeartbeatMillis = 0;
const long heartbeatInterval = 500;

// Servo
Servo gripperServo;
float currentServoAngle = 85;
int currentGripperSpeed = 0;
unsigned long previousServoMillis = 0;
const long servoUpdateInterval = 20;
const float minSpeed = 0.5;
const float maxSpeed = 3.0;

// Connection state
volatile bool deviceConnected = false;

// Pin definitions
const int LED_PIN = 2;
const int SERVO_PIN = 13;
const int PWM_PIN[3] = {25, 26, 27};
const int IN_1_PIN[3] = {33, 14, 16};
const int IN_2_PIN[3] = {32, 12, 17};
const int AUX_MOTOR_PWM = 4;
const int AUX_MOTOR_IN1 = 18;
const int AUX_MOTOR_IN2 = 19;

// Motor angles
const float THETA_DEG[3] = {0, 120, 240};
const float ROTATION_SCALE = 0.5;
