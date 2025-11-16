import type { Joystick1DProps } from "../types/component-types";
import { Joystick, JoystickShape } from "react-joystick-component";
import type { IJoystickUpdateEvent } from "react-joystick-component/build/lib/Joystick";

function Joystick1D ({ onChange, lockDirection = 'x', baseRadius = 75, ...otherProps}: Joystick1DProps) {
    
    const handleMove = (event: IJoystickUpdateEvent) => {
        if (!event.y && !event.x) return;
        
        console.log(`Y: ${event.y} X: ${event.x}`);

        // Für Y-Achse (vertikal)
        if (lockDirection === 'y') {
            const normalized = Math.round(-(event.y || 0)  * 100);
            onChange(normalized);
        } 
        // Für X-Achse (horizontal)
        else {
            const normalized = Math.round((event.x || 0) * 100);
            onChange(normalized);
        }
    };

    const handleStop = () => {
        onChange(0);
    };

    return (
        <Joystick
            size={baseRadius * 2}
            baseColor="#bdbdbdff"
            stickColor="#ffffffff"
            move={handleMove}
            stop={handleStop}
            controlPlaneShape={JoystickShape.AxisX}
            throttle={50}
            minDistance={15}
            {...otherProps}
        />
    );
}

export default Joystick1D;
