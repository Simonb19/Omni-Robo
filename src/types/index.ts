export type ConnectionStatus = {
    isConnected: boolean;
    checkConnection: () => Promise<boolean>;
    connect: () => Promise<boolean>;
    disconnect: () => Promise<void>;
    device: BluetoothDevice | null;
    controlCharacteristic: BluetoothRemoteGATTCharacteristic | null;
}

export type RobotControls = {
  gripper: number; // -100 to 100
}

export type UseRobotControlOptions = {
  heartbeatInterval?: number; // Default: 2000ms
}