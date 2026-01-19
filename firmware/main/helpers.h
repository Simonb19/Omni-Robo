#ifndef HELPERS_H
#define HELPERS_H

#include "types.h"
#include <stdint.h>

// Convert degrees to radians
float degreesToRadians(float degrees);

// Calculate force direction unit vector from angle in radians
Vector2D calculateForceDirectionUnitVector(float alpha_i_rad);

// Calculate the speed for a specific wheel based on movement vector and rotation
float getOmniWheelSpeed(int8_t x, int8_t y, int8_t omega, WheelID wheel);

// Normalize speeds to 0-100 range based on maximum value
void normalizeSpeed(float speeds[3]);

// Convert normalized speeds to motor commands
void normalizedSpeedsToMotors(float normalizedSpeeds[3], Motor motors[3]);

// Get motor commands for all three wheels based on movement input
void getMotorCommands(int8_t x, int8_t y, int8_t omega, Motor motors[3], bool omniMode);

#endif
