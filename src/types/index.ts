export type ConnectionStatus = {
  isConnected: boolean;
  checkConnection: () => Promise<boolean>;
  connect: () => Promise<boolean>;
  disconnect: () => Promise<void>;
  device: BluetoothDevice | null;
  controlCharacteristic: BluetoothRemoteGATTCharacteristic | null;
};

export type MotorDirection = 'EN1' | 'EN2'; // Rechtslauf / Linkslauf

export type Motor = {
  direction: MotorDirection;
  pwm: number;
};

export type RobotControls = {
  drive?: {
    omniMode: boolean;

    x?: number; // -100 to 100 (strafe, only used in omni mode)
    y?: number; // -100 to 100 (forward/backward)
    rotation?: number; // -100 to 100 (turn)
  };

  gripper?: {
    z?: number; // -100 to 100 (height up/down)
    openClose?: number; // -100 to 100 (open/close)
  };

  debug?: {
    motor1?: Motor;
    motor2?: Motor;
    motor3?: Motor;
  };
};

export type UseRobotControlOptions = {
  heartbeatInterval?: number; // Default: 2000ms
};
