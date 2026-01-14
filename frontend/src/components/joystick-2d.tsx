import type { Joystick2DProps } from '../types/component-types';
import { Joystick, JoystickShape } from 'react-joystick-component';
import type { IJoystickUpdateEvent } from 'react-joystick-component/build/lib/Joystick';
import { useState } from 'react';

function Joystick2D({
  onChange,
  baseRadius = 75,
  lockAxes = false,
  ...otherProps
}: Joystick2DProps) {
  const [dominantAxis, setDominantAxis] = useState<'x' | 'y' | null>(null);

  const handleMove = (event: IJoystickUpdateEvent) => {
    if (!event.y && !event.x) return;

    console.log(`Y: ${event.y} X: ${event.x}`);

    let x = event.x || 0;
    let y = event.y || 0;

    if (lockAxes) {
      // Reset dominant axis when near center (threshold: 0.2)
      const threshold = 0.2;
      if (Math.abs(x) < threshold && Math.abs(y) < threshold) {
        setDominantAxis(null);
        onChange({ x: 0, y: 0 });
        return;
      }

      // Set or maintain dominant axis
      if (dominantAxis === null) {
        // No axis locked yet, determine which one
        if (Math.abs(x) > Math.abs(y)) {
          setDominantAxis('x');
          y = 0;
        } else {
          setDominantAxis('y');
          x = 0;
        }
      } else if (dominantAxis === 'x') {
        // X axis is locked
        y = 0;
      } else {
        // Y axis is locked
        x = 0;
      }
    }

    const normalizedX = Math.round(x * 100);
    const normalizedY = Math.round(-y * 100);

    onChange({ x: normalizedX, y: normalizedY });
  };

  const handleStop = () => {
    onChange({ x: 0, y: 0 });
    setDominantAxis(null);
  };

  const getControlPlaneShape = () => {
    if (!lockAxes) return JoystickShape.Square;
    if (dominantAxis === 'x') return JoystickShape.AxisX;
    if (dominantAxis === 'y') return JoystickShape.AxisY;
    return JoystickShape.Square;
  };

  return (
    <Joystick
      size={baseRadius * 2}
      baseColor="#bdbdbdff"
      stickColor="#ffffffff"
      move={handleMove}
      stop={handleStop}
      controlPlaneShape={getControlPlaneShape()}
      throttle={50}
      minDistance={15}
      {...otherProps}
    />
  );
}

export default Joystick2D;
