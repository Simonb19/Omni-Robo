import "./control-screen.css";
import Joystick1D from "./components/joystick-1d";
import { useState } from "react";
import { useRobotControl } from "./hooks/use-robot-control";
import ConnectButton from "./components/connection-button";
import type { MotorDirection } from "./types";

function ControlScreen() {
  const [val, setVal] = useState(0);
  const [motorEnabled, setMotorEnabled] = useState(false);
  const [motorDirection, setMotorDirection] = useState<MotorDirection>("RL");

  const { sendCommand } = useRobotControl();

  // Helper to send motor command with current state
  const sendMotorCommand = (enable: boolean, direction: MotorDirection) => {
    sendCommand({
      motor1: {
        direction,
        enable,
      },
    });
  };

  const handleStart = () => {
    setMotorEnabled(true);
    sendMotorCommand(true, motorDirection);
  };

  const handleStop = () => {
    setMotorEnabled(false);
    sendMotorCommand(false, motorDirection);
  };

  const handleDirectionChange = (direction: MotorDirection) => {
    setMotorDirection(direction);
    // Only send if motor is running, otherwise just update local state
    if (motorEnabled) {
      sendMotorCommand(true, direction);
    }
  };

  return (
    <div className="control-container">
      {/* Linke Seite - Greifer */}
      <div className="control-section">
        <h2>Greifer Steuerung</h2>
        <Joystick1D
          onChange={(val) => {
            setVal(val);
            sendCommand({ gripper: val });
          }}
        />
        <p className="value-display">{val.toFixed(0)}</p>
      </div>

      {/* Trennlinie */}
      <div className="divider" />

      {/* Rechte Seite - Motor Controls */}
      <div className="control-section">
        <h2>Motor Controls</h2>

        {/* Motor Status Indicator */}
        <div className={`motor-status ${motorEnabled ? "running" : "stopped"}`}>
          {motorEnabled ? "🟢 Läuft" : "🔴 Gestoppt"} -{" "}
          {motorDirection === "RL" ? "Rechtslauf" : "Linkslauf"}
        </div>

        <div className="button-group">
          <button
            className={`control-btn start-btn ${motorEnabled ? "active" : ""}`}
            onClick={handleStart}
            disabled={motorEnabled}
          >
            ▶ Start
          </button>
          <button
            className={`control-btn stop-btn ${!motorEnabled ? "active" : ""}`}
            onClick={handleStop}
            disabled={!motorEnabled}
          >
            ⏹ Stopp
          </button>
        </div>

        <div className="direction-group">
          <button
            className={`control-btn direction-btn left-btn ${motorDirection === "LL" ? "active" : ""}`}
            onClick={() => handleDirectionChange("LL")}
          >
            ↺ Linkslauf
          </button>
          <button
            className={`control-btn direction-btn right-btn ${motorDirection === "RL" ? "active" : ""}`}
            onClick={() => handleDirectionChange("RL")}
          >
            Rechtslauf ↻
          </button>
        </div>
      </div>

      {/* Connection Status */}
      <div className="divider" />
      <div className="control-section">
        <ConnectButton />
      </div>
    </div>
  );
}

export default ControlScreen;
