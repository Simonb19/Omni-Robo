#include "helpers.h"
#include "constants.h"
#include <cmath>

// Define E_ALPHA array (declared as extern in constants.h)
Vector2D E_ALPHA[3];

void initializeConstants() {
  // Initialize E_ALPHA array with proper values
  for (int i = 0; i < 3; i++) {
    float angle_rad = degreesToRadians(THETA_DEG[i] + 225);
    E_ALPHA[i] = calculateForceDirectionUnitVector(angle_rad);
  }
}

float degreesToRadians(float degrees) {
  return degrees * M_PI / 180.0f;
}

Vector2D calculateForceDirectionUnitVector(float alpha_i_rad) {
  return {
    std::cos(alpha_i_rad),
    std::sin(alpha_i_rad)
  };
}

float getOmniWheelSpeed(int8_t x, int8_t y, int8_t omega, WheelID wheel) {
  return x * E_ALPHA[wheel].x 
       + y * E_ALPHA[wheel].y 
       + omega * ROTATION_SCALE;
}

void normalizeSpeed(float speeds[3]) {
  float m = 0;
  for (int i = 0; i < 3; i++) {
    if (std::abs(speeds[i]) > m) {
      m = std::abs(speeds[i]);
    }
  }
  
  if (m > 0) {
    for (int i = 0; i < 3; i++) {
      speeds[i] = speeds[i] * 100.0f / m;
    }
  }
}

void normalizedSpeedsToMotors(float normalizedSpeeds[3], Motor motors[3]) {
  for (int i = 0; i < 3; i++) {
    motors[i] = {
      normalizedSpeeds[i] >= 0 ? EN1 : EN2,
      static_cast<uint8_t>(std::abs(normalizedSpeeds[i]) * 255.0f / 100.0f)
    };
  }
}

void getMotorCommands(int8_t x, int8_t y, int8_t omega, Motor motors[3], bool omniMode) {
  float speeds[3];
if (omniMode) {
    for (int i = 0; i < 3; i++) {
      speeds[i] = getOmniWheelSpeed(x, y, omega, static_cast<WheelID>(i));
    }
  } else {
    // PIVOT mode - rotation around center with forward/backward
    // TODO: Implement pivot mode calculation
    // This mode should only use Y (forward/back) and omega (rotation)
    // X-axis strafing is disabled in this mode
    for (int i = 0; i < 3; i++) {
      speeds[i] = 0; // Placeholder
    }
  }
  
  normalizeSpeed(speeds);
  normalizedSpeedsToMotors(speeds, motors);
}
