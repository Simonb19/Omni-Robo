import './drive-content.css';
import Joystick1D from '@/components/joystick-1d.tsx';
import { useJoystickControls } from '@/hooks/use-joystick-controls.ts';

function GrabContent() {
  const { handleMove, handleRotation } = useJoystickControls(false);

  return (
    <div className="joystick-container">
      <div>
        <p>Vorwärts/Rückwärts</p>
        <Joystick1D
          onChange={(val) => handleMove({ x: 0, y: val })}
          lockDirection="y"
        />
      </div>
      <div>
        <p>Lenken</p>
        <Joystick1D onChange={handleRotation} />
      </div>
    </div>
  );
}

export default GrabContent;
