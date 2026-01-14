import './debug-screen.css';
import { useRobotControl } from './hooks/use-robot-control';
import ConnectButton from './components/connection-button';
import MotorTest from './components/motor-test';
import { Link } from 'react-router-dom';
import IconButton from './components/icon-button.tsx';
import { ChevronLeft } from 'lucide-react';
import type { MotorDirection } from '@/types';
import Joystick2D from '@/components/joystick-2d.tsx';
import { useJoystickControls } from '@/hooks/use-joystick-controls.ts';

function DebugScreen() {
  const { sendCommand } = useRobotControl();
  const { handleGripper } = useJoystickControls(false);

  const handleMotorCommand = (
    motorId: 1 | 2 | 3,
    enable: boolean,
    direction: MotorDirection
  ) => {
    const motorKey = `motor${motorId}` as 'motor1' | 'motor2' | 'motor3';
    sendCommand({
      debug: {
        [motorKey]: {
          direction,
          enable,
        },
      },
    });
  };

  return (
    <div className="debug-screen-wrapper">
      <Link to={'/'}>
        <IconButton icon={ChevronLeft} className="icon-button-top-left" />
      </Link>
      <div className="control-container">
        {/* Greifer Steuerung */}
        <div className="control-section">
          <h2>Greifer Steuerung</h2>
          <Joystick2D onChange={handleGripper} lockAxes />
        </div>

        <div className="divider" />

        {/* Motor Controls - All 3 side by side */}
        <div className="motors-section">
          <MotorTest motorId={1} onMotorCommand={handleMotorCommand} />
          <MotorTest motorId={2} onMotorCommand={handleMotorCommand} />
          <MotorTest motorId={3} onMotorCommand={handleMotorCommand} />
        </div>

        <div className="divider" />

        {/* Connection Status */}
        <div className="control-section">
          <ConnectButton />
        </div>
      </div>
    </div>
  );
}

export default DebugScreen;
