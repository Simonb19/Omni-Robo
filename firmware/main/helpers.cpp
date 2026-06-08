// ============================================================
//  Umsetzung der Omni-Wheel-Kinematik (siehe helpers.h).
// ============================================================

#include "helpers.h"
#include "constants.h"
#include <cmath>

// Richtungs-Einheitsvektoren je Rad (in constants.h als extern bekannt).
Vector2D E_ALPHA[3];

void initializeConstants() {
  // Für jedes Rad aus dem Einbauwinkel den Richtungsvektor vorberechnen,
  // damit das im Fahrbetrieb nicht jedes Mal neu gerechnet werden muss.
  for (int i = 0; i < 3; i++) {
    float angle_rad = degreesToRadians(THETA_DEG[i]);
    E_ALPHA[i] = calculateForceDirectionUnitVector(angle_rad);
  }
}

float degreesToRadians(float degrees) {
  return degrees * M_PI / 180.0f;
}

Vector2D calculateForceDirectionUnitVector(float alpha_i_rad) {
  return {
    std::sin(alpha_i_rad),  // x-Komponente
    std::cos(alpha_i_rad)   // y-Komponente
  };
}

float getOmniWheelSpeed(int8_t x, int8_t y, int8_t omega, WheelID wheel) {
  // Projektion des Bewegungsvektors auf die Radrichtung + Drehanteil.
  return x * E_ALPHA[wheel].x
       + y * E_ALPHA[wheel].y
       + omega * ROTATION_SCALE;
}

void normalizeSpeed(float speeds[3]) {
  // Größten Betrag suchen.
  float m = 0;
  for (int i = 0; i < 3; i++) {
    if (std::abs(speeds[i]) > m) {
      m = std::abs(speeds[i]);
    }
  }

  // Nur skalieren, wenn ein Rad über 100 liegt (Verhältnis bleibt erhalten).
  if (m > 100.0f) {
    for (int i = 0; i < 3; i++) {
      speeds[i] = speeds[i] * 100.0f / m;
    }
  }
}

void normalizedSpeedsToMotors(float normalizedSpeeds[3], Motor motors[3]) {
  for (int i = 0; i < 3; i++) {
    motors[i] = {
      // Vorzeichen bestimmt die Drehrichtung ...
      normalizedSpeeds[i] >= 0 ? EN1 : EN2,
      // ... Betrag wird von 0..100 auf 0..255 (PWM) umgerechnet.
      static_cast<uint8_t>(std::abs(normalizedSpeeds[i]) * 255.0f / 100.0f)
    };
  }
}

void getMotorCommands(int8_t x, int8_t y, int8_t omega, Motor motors[3], bool omniMode) {
  float speeds[3];

  if (omniMode) {
    // Omni-Modus: volle 2D-Bewegung + Drehung.
    for (int i = 0; i < 3; i++) {
      speeds[i] = getOmniWheelSpeed(x, y, omega, static_cast<WheelID>(i));
    }
  } else {
    // Pivot-Modus: nur vor/zurück (x) und drehen (als y übergeben), kein Strafen.
    for (int i = 0; i < 3; i++) {
      speeds[i] = getOmniWheelSpeed(x, omega, 0, static_cast<WheelID>(i));
    }
  }

  normalizeSpeed(speeds);
  normalizedSpeedsToMotors(speeds, motors);
}

// ------------------------------------------------------------
//  Such-Animation (Knight-Rider-Lauflicht) für die Gehäuse-LEDs.
//  Non-blocking über millis().
// ------------------------------------------------------------
static unsigned long previousSearchMillis = 0;
static const long searchInterval = 120;   // kleiner = schneller
static int searchIndex = 0;               // aktuelle LED-Position
static int searchDir = 1;                 // 1 = vor, -1 = zurueck

void updateSearchAnimation() {
  unsigned long now = millis();
  if (now - previousSearchMillis < searchInterval) return;
  previousSearchMillis = now;

  // Nur die aktuelle LED an, der Rest aus.
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PINS[i], (i == searchIndex) ? HIGH : LOW);
  }

  // Position weiterschieben und am Rand umkehren (hin und her).
  searchIndex += searchDir;
  if (searchIndex >= 3) { searchIndex = 3; searchDir = -1; }
  if (searchIndex <= 0) { searchIndex = 0; searchDir =  1; }
}

void setLedsConnected() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PINS[i], HIGH);   // alle an = verbunden
  }
}

