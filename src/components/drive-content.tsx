import './drive-content.css';
import Joystick1D from '@/components/joystick-1d.tsx';

function GrabContent() {
  return (
    <div className="joystick-container">
      <div>
        <p>Vorwärts/Rückwärts</p>
        <Joystick1D onChange={() => {}} lockDirection="y" />
      </div>
      <div>
        <p>Lenken</p>
        <Joystick1D onChange={() => {}} />
      </div>
    </div>
  );
}

export default GrabContent;
