import { useState } from 'react';
import type { MotorDirection } from '@/types';
import './motor-test.css';
import type { MotorTestProps } from '@/types/component-types.ts';

const DEFAULT_PWM = 255;

function MotorTest({ motorId, onMotorCommand }: MotorTestProps) {
  const [motorEnabled, setMotorEnabled] = useState(false);
  const [motorDirection, setMotorDirection] = useState<MotorDirection>('EN1');

  const handleStart = () => {
    setMotorEnabled(true);
    onMotorCommand(motorId, DEFAULT_PWM, motorDirection);
  };

  const handleStop = () => {
    setMotorEnabled(false);
    onMotorCommand(motorId, 0, motorDirection);
  };

  const handleDirectionChange = (direction: MotorDirection) => {
    setMotorDirection(direction);
    if (motorEnabled) {
      onMotorCommand(motorId, DEFAULT_PWM, direction);
    }
  };

  return (
    <div className="motor-test">
      <h2>Motor {motorId}</h2>

      <div className={`motor-status ${motorEnabled ? 'running' : 'stopped'}`}>
        {motorEnabled ? '🟢 Läuft' : '🔴 Gestoppt'} –{' '}
        {motorDirection === 'EN1' ? 'Rechtslauf' : 'Linkslauf'}
      </div>

      <div className="button-group">
        <button
          className={`control-btn start-btn ${motorEnabled ? 'active' : ''}`}
          onClick={handleStart}
          disabled={motorEnabled}
        >
          ▶ Start
        </button>
        <button
          className={`control-btn stop-btn ${!motorEnabled ? 'active' : ''}`}
          onClick={handleStop}
          disabled={!motorEnabled}
        >
          ⏹ Stopp
        </button>
      </div>

      <div className="direction-group">
        <button
          className={`control-btn direction-btn left-btn ${motorDirection === 'EN2' ? 'active' : ''}`}
          onClick={() => handleDirectionChange('EN2')}
        >
          ↺ Linkslauf
        </button>
        <button
          className={`control-btn direction-btn right-btn ${motorDirection === 'EN1' ? 'active' : ''}`}
          onClick={() => handleDirectionChange('EN1')}
        >
          Rechtslauf ↻
        </button>
      </div>
    </div>
  );
}

export default MotorTest;
