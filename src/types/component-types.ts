import type { IJoystickProps } from "rc-joystick"

export type Joystick1DProps = {
    onChange: (val: number) => void,
    lockDirection?: 'y' | 'x',
} & Omit<IJoystickProps, 'onChange'>