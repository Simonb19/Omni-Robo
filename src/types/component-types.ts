import type { IJoystickProps } from 'react-joystick-component/build/lib/Joystick';
import type { LucideIcon } from 'lucide-react';

export type Joystick1DProps = {
  onChange: (val: number) => void;
  lockDirection?: 'y' | 'x';
  baseRadius?: number;
} & Omit<IJoystickProps, 'onChange'>;

export type Joystick2DProps = {
  onChange: (val: { x: number; y: number }) => void;
  baseRadius?: number;
  lockAxes?: boolean;
} & Omit<IJoystickProps, 'onChange'>;

export type IconButtonProps = {
  icon: LucideIcon;
  size?: number;
} & React.ComponentProps<'button'>;
