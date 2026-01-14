#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"
#include <math.h>

// ============================================
// BLE UUIDs
// ============================================
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CONTROL_CHAR_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define STATUS_CHAR_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a9"

// ============================================
// Pin Definitions
// ============================================
#define LED_PIN  2       // REMOVED SEMICOLON
#define SERVO_PIN  13    // REMOVED SEMICOLON

const int PWM_PIN[3]  = {12, 26, 32};
const int IN_1_PIN[3] = {14, 25, 22};
const int IN_2_PIN[3] = {27, 33, 23};

// ============================================
// Physical Parameters Definitions
// ============================================

const float ROTATION_SCALE = 1.0;

const int THETA_DEG[3] = {90, 210, 330};

// E_ALPHA will be initialized at runtime
extern Vector2D E_ALPHA[3];

// Function to initialize E_ALPHA (call this in setup())
void initializeConstants();

#endif
