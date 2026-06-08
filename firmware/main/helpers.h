// ============================================================
//  Kinematik-Helfer: rechnet einen Fahrbefehl (x, y, Drehung)
//  in die Geschwindigkeiten der 3 Omni-Räder um.
// ============================================================

#ifndef HELPERS_H
#define HELPERS_H

#include "types.h"
#include <stdint.h>

/**
 * Wandelt Grad in Radiant um.
 * @param degrees  Winkel in Grad
 * @return         Winkel in Radiant
 */
float degreesToRadians(float degrees);

/**
 * Liefert den Richtungs-Einheitsvektor eines Rades aus seinem Winkel.
 * @param alpha_i_rad  Einbauwinkel des Rades in Radiant
 * @return             Einheitsvektor (sin, cos)
 */
Vector2D calculateForceDirectionUnitVector(float alpha_i_rad);

/**
 * Berechnet die Geschwindigkeit eines einzelnen Rades.
 * Formel: v = x*sin(alpha) + y*cos(alpha) + omega
 * @param x      seitliche Bewegung (-100..100)
 * @param y      Vorwärtsbewegung (-100..100)
 * @param omega  Drehung (-100..100)
 * @param wheel  ID des Rades (WHEEL_1..3)
 * @return       Rohgeschwindigkeit (kann >100 sein, wird später normiert)
 */
float getOmniWheelSpeed(int8_t x, int8_t y, int8_t omega, WheelID wheel);

/**
 * Skaliert alle Radgeschwindigkeiten auf max. 100 herunter,
 * falls der größte Betrag 100 überschreitet (behält Verhältnis bei).
 * @param speeds  Array mit 3 Radgeschwindigkeiten (wird verändert)
 */
void normalizeSpeed(float speeds[3]);

/**
 * Wandelt die normierten Geschwindigkeiten in Motorbefehle um
 * (Vorzeichen -> Richtung, Betrag -> PWM 0..255).
 * @param normalizedSpeeds  Array mit 3 Geschwindigkeiten (-100..100)
 * @param motors            Ausgabe: 3 Motorbefehle
 */
void normalizedSpeedsToMotors(float normalizedSpeeds[3], Motor motors[3]);

/**
 * Kompletter Ablauf: aus einem Fahrbefehl die Befehle für alle
 * 3 Motoren berechnen (Kinematik + Normierung + Umrechnung).
 * @param x         seitliche Bewegung
 * @param y         Vorwärtsbewegung
 * @param omega     Drehung
 * @param motors    Ausgabe: 3 Motorbefehle
 * @param omniMode  true = omnidirektional, false = Pivot/Drehen
 */
void getMotorCommands(int8_t x, int8_t y, int8_t omega, Motor motors[3], bool omniMode);

/**
 * Such-Animation (Lauflicht) für die Gehäuse-LEDs.
 * Aufruf in loop(), solange keine Verbindung besteht. Non-blocking.
 */
void updateSearchAnimation();

/**
 * Setzt die Gehäuse-LEDs auf den "verbunden"-Zustand (alle an).
 */
void setLedsConnected();
#endif
