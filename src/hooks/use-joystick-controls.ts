import { useRef, useCallback } from 'react';
import { useRobotControl } from './use-robot-control';

export const useJoystickControls = (omniMode: boolean) => {
  const { sendCommand, isConnected } = useRobotControl();

  const driveRef = useRef({ x: 0, y: 0 });
  const rotationRef = useRef(0);
  const gripperRef = useRef({ z: 0, openClose: 0 });

  const sendDriveCommand = useCallback(() => {
    sendCommand({
      drive: {
        omniMode: omniMode,
        x: driveRef.current.x,
        y: driveRef.current.y,
        rotation: rotationRef.current,
      },
    });
  }, [sendCommand, omniMode]);

  const sendGripperCommand = useCallback(() => {
    sendCommand({
      gripper: {
        z: gripperRef.current.z,
        openClose: gripperRef.current.openClose,
      },
    });
  }, [sendCommand]);

  const handleMove = useCallback(
    (val: { x: number; y: number }) => {
      driveRef.current = val;
      sendDriveCommand();
    },
    [sendDriveCommand]
  );

  const handleRotation = useCallback(
    (val: number) => {
      rotationRef.current = val;
      sendDriveCommand();
    },
    [sendDriveCommand]
  );

  const handleGripper = useCallback(
    (val: { x: number; y: number }) => {
      gripperRef.current = {
        openClose: val.x,
        z: val.y,
      };
      sendGripperCommand();
    },
    [sendGripperCommand]
  );

  return {
    handleMove,
    handleRotation,
    handleGripper,
    isConnected,
  };
};
