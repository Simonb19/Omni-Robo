export type ConnectionStatus = {
  isConnected: boolean;
  checkConnection: () => Promise<boolean>;
  connect: () => Promise<boolean>;
  disconnect: () => Promise<void>;
  device: BluetoothDevice | null;
  controlCharacteristic: BluetoothRemoteGATTCharacteristic | null;
};

export type MotorDirection = "RL" | "LL"; // Rechtslauf / Linkslauf

export type Motor = {
  direction: MotorDirection;
  enable: boolean;
};

export type RobotControls = {
  gripper?: number; // -100 to 100
  motor1?: Motor;
};

export type UseRobotControlOptions = {
  heartbeatInterval?: number; // Default: 2000ms
};
