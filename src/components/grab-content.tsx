import './grab-content.css';
import Joystick2D from '@/components/joystick-2d.tsx';
import { useState } from 'react';

function GrabContent() {
  const [text, setText] = useState('');
  return (
    <div className="joystick-container">
      <div>
        <p>Bewegen</p>
        <Joystick2D onChange={() => {}} />
      </div>
      <div>
        <p>Drehen - Auf/Nieder</p>
        <p>{text}</p>
        <Joystick2D
          onChange={(val) => setText(`X: ${val.x}, Y: ${val.y}`)}
          lockAxes
        />
      </div>
    </div>
  );
}

export default GrabContent;
