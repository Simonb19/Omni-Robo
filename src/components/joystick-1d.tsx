import type { Joystick1DProps } from "../types/component-types";
import Joystick, { Direction, type IJoystickChangeValue } from 'rc-joystick';

function Joystick1D ({ onChange, lockDirection = 'x', baseRadius = 75, ...otherProps}:Joystick1DProps) {

    return (
        <Joystick 
        lockX={lockDirection === 'x'}
        lockY={lockDirection === 'y'}
        throttle={50}
        onChange={(val: IJoystickChangeValue) => {
            const normalized = val.direction === Direction.Top 
            ? val.distance * 100 / baseRadius
            : -(val.distance * 100 / baseRadius) || 0;

            onChange(normalized);
        }}
        {...otherProps}
        />
    )
}

export default Joystick1D