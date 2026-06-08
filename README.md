<div align="center">

# 🤖 Omni-Robo

### Ein omnidirektionaler Roboter mit Greifarm – gesteuert direkt aus dem Browser über Bluetooth.

<img src="docs/cad-cover.png" width="560" alt="Omni-Robo CAD-Render">

<br><br>

[![Live Demo](https://img.shields.io/badge/Live_Demo-Steuerung_öffnen-2ea44f?style=for-the-badge&logo=googlechrome&logoColor=white)](https://simonb19.github.io/Omni-Robo/)

<br>

![React](https://img.shields.io/badge/React-20232A?style=flat-square&logo=react&logoColor=61DAFB)
![TypeScript](https://img.shields.io/badge/TypeScript-3178C6?style=flat-square&logo=typescript&logoColor=white)
![Vite](https://img.shields.io/badge/Vite-646CFF?style=flat-square&logo=vite&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=flat-square&logo=espressif&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=flat-square&logo=cplusplus&logoColor=white)
![Bluetooth](https://img.shields.io/badge/Web_Bluetooth-0082FC?style=flat-square&logo=bluetooth&logoColor=white)

</div>

---

## Über das Projekt

Omni-Robo ist ein ferngesteuerter Roboter, den wir in unserem KOP-Projekt im 4. Jahrgang an der HTL Steyr gebaut haben. Er fährt omnidirektional über drei Räder, kann sich also in jede Richtung bewegen und gleichzeitig drehen. Oben drauf sitzt ein Greifarm an einer Spindel, mit dem er Sachen aufheben, hochfahren und woanders wieder absetzen kann.

Gesteuert wird er über eine Website, die sich per Bluetooth direkt mit dem Roboter verbindet. Man muss also nichts installieren – Seite öffnen, verbinden, losfahren.

In diesem Repo liegt nur die Software, also die Steuerungs-Website und die Firmware für den ESP32. Die Mechanik haben wir in Creo konstruiert und großteils 3D-gedruckt, die Platinen in KiCad gezeichnet. Diese Dateien sind hier nicht dabei, aber weiter unten sind ein paar Bilder davon.

---



<div align="center">

[![Demo-Video ansehen](docs/video-thumbnail.png)](VIDEO_LINK_HIER_EINFÜGEN)



</div>

---

## Was er kann

Fahren funktioniert über zwei Joysticks – einer für die Bewegung, einer fürs Drehen. Der Greifer wird über einen eigenen Joystick gesteuert: zur Seite öffnet und schließt er (Servo), nach oben und unten fährt der Greifarm (Schrittmotor). Die ganze Verbindung läuft über Bluetooth direkt aus dem Browser.

Der Roboter meldet während der Fahrt laufend seinen Status zurück. Wenn die Verbindung abbricht, bleibt er von selbst stehen, damit er nicht unkontrolliert weiterfährt. Zum Testen gibt es außerdem einen Debug-Modus, in dem man jeden Motor einzeln ansteuern kann.

---

## Wie die Steuerung funktioniert

Die Website verbindet sich per Web Bluetooth direkt mit dem ESP32. Sobald man einen Joystick bewegt, schickt sie einen kleinen JSON-Befehl an den Roboter. Damit die Funkverbindung nicht überlastet wird, wird immer nur der aktuellste Befehl gesendet und gleiche Befehle werden übersprungen. Der ESP32 rechnet daraus die Geschwindigkeit für jedes der drei Räder aus und steuert die Motoren, den Schrittmotor und den Servo entsprechend an. Parallel dazu schickt er regelmäßig seinen Status zurück – bleibt diese Rückmeldung aus, stoppt er automatisch.

<div align="center">
<img src="docs/architecture.png" width="760" alt="Aufbau der Steuerung">
</div>

Web Bluetooth funktioniert übrigens mit Chrome und Edge (am PC und unter Android), aber nicht mit Safari bzw. auf dem iPhone.

---

## Technik

Die Website ist mit React und TypeScript gebaut, als Build-Tool kommt Vite zum Einsatz. Die Verbindung zum Roboter läuft über die Web-Bluetooth-API, deshalb geht alles direkt im Browser ohne extra App. Für die Oberfläche haben wir Tailwind und shadcn/ui verwendet, die Joysticks kommen von react-joystick-component und die Navigation zwischen Steuerung und Debug-Ansicht macht der React Router. Deployed ist die Seite über GitHub Pages.

Die Firmware am ESP32 ist in C++ mit dem Arduino-Framework geschrieben. Sie nimmt die Befehle per Bluetooth (BLE) entgegen, liest das JSON mit ArduinoJson aus und steuert damit die Antriebsmotoren, den Schrittmotor für die Höhe und den Servo am Greifer. Die Umrechnung von der Joystick-Eingabe auf die Geschwindigkeit der einzelnen Räder übernimmt eine eigene Kinematik-Funktion.

An Hardware steckt dahinter ein ESP32, zwei L293D-H-Brücken für die drei Fahrmotoren, ein A4988-Treiber für den Schrittmotor der Höhenachse, ein Servo für den Greifer sowie ein paar Status- und Akku-LEDs.

---

## Schaltpläne

Wir haben drei Platinen entworfen: die Hauptplatine mit dem ESP32, die Treiberplatine mit den Motortreibern und eine kleine Platine für die Akku-LEDs.

<div align="center">
<table>
<tr>
<td align="center"><img src="docs/schaltplan-p1.png" width="270"><br><sub><b>Hauptplatine</b> · ESP32</sub></td>
<td align="center"><img src="docs/schaltplan-p2.png" width="270"><br><sub><b>Treiberplatine</b> · A4988 + L293D</sub></td>
<td align="center"><img src="docs/schaltplan-p3.png" width="270"><br><sub><b>LED-Platine</b></sub></td>
</tr>
</table>
</div>

---

## Platinen (3D-Ansicht aus KiCad)

<div align="center">
<table>
<tr>
<td align="center"><img src="docs/platine-1-render.png" width="270"><br><sub><b>Hauptplatine</b></sub></td>
<td align="center"><img src="docs/platine-2-render.png" width="270"><br><sub><b>Treiberplatine</b></sub></td>
<td align="center"><img src="docs/platine-3-render.png" width="270"><br><sub><b>LED-Platine</b></sub></td>
</tr>
</table>
</div>

---

## Mechanik

<div align="center">
<img src="docs/cad-explosion-view.jpg" width="330">
<img src="docs/cad-cover.png.jpg" width="330">
</div>

Die Mechanik haben wir in Creo konstruiert und großteils im 3D-Druck gefertigt. Ein dreieckiger Grundrahmen trägt die drei Räder und in der Mitte eine senkrechte Spindel, an der der Greifer rauf- und runterfährt.

---

## Team

Das Projekt ist als Gruppenarbeit zu fünft entstanden:

- Wendelin – Konstruktion / Mechanik
- Felix – Konstruktion / Mechanik / Dokumentation
- Joshua – Konstruktion / Elektronik
- Simon – Elektronik / Software 
- Lukas – Elektronik / Software

<div align="center">

<br>

**[▶ Zur Steuerung](https://simonb19.github.io/Omni-Robo/)**

<sub>HTL Steyr · KOP-Projekt · 4. Jahrgang</sub>

</div>
