<div align="center">
 
# 🤖 Omni-Robo
 
**Ein omnidirektionaler Roboter mit Greifarm – gesteuert direkt aus dem Browser über Bluetooth.**
 
<!-- COVER: CAD-Render des Roboters hier einfügen (z. B. aus Creo exportiert) -->
<img src="docs/cad-cover.png" alt="Omni-Robo CAD-Render" width="520">
 
[![Live Demo](https://img.shields.io/badge/▶_Live_Demo-Steuerungs--App-2ea44f?style=for-the-badge)](https://simonb19.github.io/Omni-Robo/)
 
React · TypeScript · Vite · Web Bluetooth · ESP32 · C++
 
</div>
 
---
 
## 📖 Über das Projekt
 
Der **Omni-Robo** ist ein an der **HTL Steyr** (4. Jahrgang, KOP-Projekt) entwickelter, ferngesteuerter Roboter. Er besitzt einen **omnidirektionalen Antrieb mit drei um 120° versetzten Omni-Rädern**, wodurch er sich in jede Richtung bewegen und gleichzeitig drehen kann. Über eine vertikale Spindel (Z-Achse) und einen Servo-Greifer kann er zylindrische Bauteile aufnehmen, anheben und an anderer Stelle wieder absetzen.
 
Gesteuert wird der Roboter **ohne App-Installation** über eine Web-App, die sich per **Web Bluetooth** direkt mit dem ESP32 verbindet.
 
> 🏆 **Challenge:** In einem Parcours musste eine Cola-Dose gegriffen und auf eine zweite gestapelt sowie ein Tischtennisball in ein Ziel befördert werden – auf Zeit.
 
**Dieses Repository enthält ausschließlich die Software** (Steuerungs-App + ESP32-Firmware). Die Mechanik wurde in **PTC Creo** konstruiert, die Elektronik/Platinen in **KiCad** – diese Dateien sind hier nicht enthalt­en, die Ergebnisse sind unten als Bilder dokumentiert.
 
---
 
## 🎥 Demo-Video
 
<!-- VIDEO: Hier den Link zum Fahr-Video einfügen (z. B. YouTube) und das Vorschaubild ersetzen -->
<div align="center">
 
[![Demo-Video ansehen](docs/video-thumbnail.png)](VIDEO_LINK_HIER_EINFÜGEN)
 
*▶️ Klicken, um den Roboter in Aktion zu sehen*
 
</div>
 
---
 
## ✨ Funktionen
 
- **Omnidirektionales Fahren** – Bewegung in jede Richtung über zwei Joysticks (Fahren + Drehen)
- **Greifer-Steuerung** – Öffnen/Schließen (Servo) und Höhe auf/ab (Schrittmotor, Z-Achse)
- **Live-Verbindung per Bluetooth** – direkt aus dem Browser, ohne Installation
- **Status-Rückmeldung** – der Roboter sendet zyklisch seinen Zustand zurück (Heartbeat); bei Verbindungsverlust wird automatisch ein Not-Aus ausgelöst
- **Debug-Modus** – jeder Motor kann einzeln getestet werden
 
---
 
## 🛠️ Tech Stack
 
**Steuerungs-App (Frontend)**
- **React** + **TypeScript**
- **Vite** als Build-Tool
- **Web Bluetooth API** – Verbindung zum Roboter direkt im Browser
- **React Router** – Wechsel zwischen Steuerungs- und Debug-Ansicht
- **Tailwind CSS** + **shadcn/ui** – UI-Komponenten
- **react-joystick-component** – die virtuellen Joysticks
- Deployment über **GitHub Pages**
 
**Firmware (Backend, ESP32)**
- **C++** im **Arduino-Framework**
- **BLE (GATT-Server)** zur Kommunikation mit der App
- **ArduinoJson** zum Parsen der Steuerbefehle
- **ESP32Servo** für den Greifer
- Eigene **Omni-Wheel-Kinematik** zur Umrechnung der Joystick-Eingaben in Radgeschwindigkeiten
 
**Hardware**
- ESP32 · 2× L293D H-Brücken (3 DC-Motoren) · A4988 Schrittmotortreiber (NEMA 17, Z-Achse) · Servo (Greifer) · Status- und Akku-LEDs
 
---
 
## ⚙️ Wie es funktioniert
 
```
┌──────────────┐   Web Bluetooth (BLE)   ┌──────────────┐
│  Browser-App │ ──── JSON-Befehle ────► │    ESP32     │
│  (React)     │ ◄─── Status/Heartbeat ─ │  (Firmware)  │
└──────────────┘                         └──────┬───────┘
                                                │
                       ┌────────────────────────┼────────────────────────┐
                       ▼                         ▼                        ▼
                  DC-Motoren                 Schrittmotor              Servo
                (3× Omni-Rad,                 (Z-Achse /              (Greifer
                 H-Brücke)                     Höhe)                  auf/zu)
```
 
1. Die Web-App verbindet sich per **Web Bluetooth** direkt mit dem ESP32 (`navigator.bluetooth`).
2. Joystick-Bewegungen werden als **JSON-Befehle** über eine BLE-Characteristic an den Roboter geschickt. Eine kleine Warteschlange sorgt dafür, dass immer nur der **aktuellste** Befehl gesendet und Duplikate übersprungen werden – das entlastet die Funkverbindung.
3. Die Firmware **parst das JSON**, rechnet im Fahrbetrieb über die **Omni-Kinematik** die Geschwindigkeit jedes der drei Räder aus und steuert Motoren, Schrittmotor und Servo an.
4. Der ESP32 meldet zyklisch seinen **Status** zurück. Bleibt diese Rückmeldung aus (Verbindungsabbruch), stoppt der Roboter automatisch alle Motoren.
 
> 💡 **Hinweis:** Web Bluetooth wird von **Chrome/Edge** (Desktop & Android) unterstützt – nicht von Safari/iOS.
 
---
 
## 🔌 Elektronik (KiCad)
 
Die drei Platinen wurden in KiCad entworfen. Schaltpläne und 3D-Renders:
 
### Schaltpläne
 
| Hauptplatine (ESP32) | Motortreiber (A4988 + L293D) | LED-Platine |
|:---:|:---:|:---:|
| ![Schaltplan Platine 1](docs/schaltplan-p1.png) | ![Schaltplan Platine 2](docs/schaltplan-p2.png) | ![Schaltplan Platine 3](docs/schaltplan-p3.png) |
 
### Platinen – 3D-Render
 
| Hauptplatine | Treiberplatine | LED-Platine |
|:---:|:---:|:---:|
| ![Render Platine 1](docs/platine-1-render.png) | ![Render Platine 2](docs/platine-2-render.png) | ![Render Platine 3](docs/platine-3-render.png) |
 
---
 
## 🏗️ Mechanik & Aufbau (Creo)
 
<!-- Echte Fotos vom fertigen Roboter hier einfügen -->
<div align="center">
 
<img src="docs/robot-1.jpg" alt="Omni-Robo Foto 1" width="320">
<img src="docs/robot-2.jpg" alt="Omni-Robo Foto 2" width="320">
 
</div>
 
Die Mechanik wurde in **PTC Creo** konstruiert und überwiegend im **3D-Druck** gefertigt: ein dreieckiger Grundrahmen trägt die drei Omni-Räder sowie eine zentrale vertikale Spindel, an der ein linear geführter Schlitten mit Greifer auf- und abfährt.
 
---
 
## 📁 Projektstruktur
 
```
Omni-Robo/
├── firmware/        # ESP32-Firmware (C++ / Arduino)
│   └── main/        # BLE, Kinematik, Motor-/Servo-/Stepper-Steuerung
└── frontend/        # Steuerungs-App (React + TypeScript + Vite)
    └── src/         # Screens, Komponenten, Hooks, BLE-Verbindung
```
 
---
 
## 👥 Team
 
Entstanden als Gruppenprojekt (5 Personen) an der HTL Steyr:
 
| Name | Rolle |
|---|---|
| [Name 1] | Konstruktion / Mechanik |
| [Name 2] | Konstruktion / Mechanik / Dokumentation |
| [Name 3] | Konstruktion / Elektronik |
| [Name 4] *(Teamleitung)* | Elektronik / Software |
| [Name 5] | Elektronik / Software |
 
---
 
<div align="center">
 
**[▶ Zur Steuerungs-App](https://simonb19.github.io/Omni-Robo/)**
 
HTL Steyr · KOP-Projekt · 4. Jahrgang
 
</div>
