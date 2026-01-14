import './grab-content.css';
import Joystick2D from '@/components/joystick-2d.tsx';
import Joystick1D from '@/components/joystick-1d.tsx';
import { useJoystickControls } from '@/hooks/use-joystick-controls.ts';

function GrabContent() {
  const { handleMove, handleRotation, handleGripper } =
    useJoystickControls(true);

  return (
    <div className="joystick-container">
      <div className="left-controls">
        <p>Bewegen</p>
        <Joystick2D onChange={handleMove} />
      </div>
      <div className="right-controls">
        <div className="control-group">
          <div className="horizontal-joystick">
            <Joystick2D onChange={handleGripper} lockAxes />
            <p>Greifer Hoch/Runter</p>
          </div>
          <p>Greifer Auf/Zu</p>
        </div>
        <div className="control-group">
          <Joystick1D onChange={handleRotation} />
          <p>Drehen</p>
        </div>
      </div>
    </div>
  );
}

export default GrabContent;
