import './control-screen.css'
import Joystick1D from './components/joystick-1d';
import { useState } from 'react';
import { useRobotControl } from './hooks/use-robot-control';

function ControlScreen() {
  const [val, setVal] = useState(0);
  const { sendCommand } = useRobotControl();


  return (
    <>
      <div className='flex gap-4 flex-col'>
        <p>Greifer Steuerung</p>
          <Joystick1D onChange={(val) => {
              setVal(val)
              sendCommand({gripper: val})
              }
            }
          />
        <p>{val.toFixed(0)}</p>
      </div>
    </>
  )
}

export default ControlScreen;
