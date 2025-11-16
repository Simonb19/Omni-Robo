import type { IJoystickProps } from "react-joystick-component/build/lib/Joystick";

export type Joystick1DProps = {
    onChange: (val: number) => void,
    lockDirection?: 'y' | 'x',
    baseRadius?: number,
} & Omit<IJoystickProps, 'onChange'>