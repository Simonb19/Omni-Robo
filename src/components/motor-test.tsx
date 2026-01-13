import { useState } from 'react';
import type { MotorDirection } from '@/types';
import './motor-test.css';
import type { MotorTestProps } from '@/types/component-types.ts';

function MotorTest({ motorId, onMotorCommand }: MotorTestProps) {
  const [motorEnabled, setMotorEnabled] = useState(false);
  const [motorDirection, setMotorDirection] = useState<MotorDirection>('RL');

  const handleStart = () => {
    setMotorEnabled(true);
    onMotorCommand(motorId, true, motorDirection);
  };

  const handleStop = () => {
    setMotorEnabled(false);
    onMotorCommand(motorId, false, motorDirection);
  };

  const handleDirectionChange = (direction: MotorDirection) => {
    setMotorDirection(direction);
    if (motorEnabled) {
      onMotorCommand(motorId, true, direction);
    }
  };

  return (
    <div className="motor-test">
      <h2>Motor {motorId}</h2>

      <div className={`motor-status ${motorEnabled ? 'running' : 'stopped'}`}>
        {motorEnabled ? '🟢 Läuft' : '🔴 Gestoppt'} –{' '}
        {motorDirection === 'RL' ? 'Rechtslauf' : 'Linkslauf'}
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
          className={`control-btn direction-btn left-btn ${motorDirection === 'LL' ? 'active' : ''}`}
          onClick={() => handleDirectionChange('LL')}
        >
          ↺ Linkslauf
        </button>
        <button
          className={`control-btn direction-btn right-btn ${motorDirection === 'RL' ? 'active' : ''}`}
          onClick={() => handleDirectionChange('RL')}
        >
          Rechtslauf ↻
        </button>
      </div>
    </div>
  );
}

export default MotorTest;
