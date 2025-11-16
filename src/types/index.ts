export type ConnectionStatus = {
    isConnected: boolean,
    checkConnection: () => void
}

export type RobotControls = {
  gripper: number; // -100 to 100
}

export type UseRobotControlOptions = {
  heartbeatInterval?: number; // Default: 2000ms
}