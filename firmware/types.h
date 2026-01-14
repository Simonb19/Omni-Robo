#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// ============================================
// General Types
// ============================================

struct Vector2D {
  float x;
  float y;
};

// types.h
typedef enum {
  WHEEL_1 = 0,
  WHEEL_2 = 1,
  WHEEL_3 = 2
} WheelID;


// ============================================
// Motor Types
// ============================================
enum MotorDirection {
  EN1,  // Direction 1
  EN2   // Direction 2
};

struct Motor {
  MotorDirection direction;
  uint8_t pwm;
};

// ============================================
// Input Types
// ============================================
struct DriveInput {
  bool omniMode;
  int8_t x;        // -100 to 100 (ignored in PIVOT mode)
  int8_t y;        // -100 to 100
  int8_t rotation; // -100 to 100
};

struct Gripper {
  int8_t openClose;  // -100 to 100
  int8_t z;          // -100 to 100 (height)
};

// ============================================
// Robot Controls
// ============================================
struct RobotControls {
  DriveInput drive;
  Gripper gripper;

  bool debugMode;
  Motor motors[3];

  unsigned long lastUpdate;
};

// ============================================
// Default Initializers
// ============================================
inline RobotControls createDefaultControls() {
  RobotControls ctrl;
  ctrl.drive.omniMode = false;
  ctrl.drive.x = 0;
  ctrl.drive.y = 0;
  ctrl.drive.rotation = 0;
  ctrl.gripper.openClose = 0;
  ctrl.gripper.z = 0;
  ctrl.debugMode = false;

  for (int i = 0; i < 3; i++) {
    ctrl.motors[i].direction = EN1;
    ctrl.motors[i].pwm = 0;
  }

  ctrl.lastUpdate = 0;
  return ctrl;
}

#endif // TYPES_H
