import { useCallback, useRef } from 'react';
import type { RobotControls } from '@/types';
import { useConnection } from './use-connection';

export const useRobotControl = () => {
  const { isConnected, controlCharacteristic } = useConnection();

  const lastCommandTimeRef = useRef<number>(0);
  const lastSentControlsRef = useRef<RobotControls | null>(null);
  const commandQueueRef = useRef<RobotControls[]>([]);
  const isProcessingRef = useRef(false);

  const processQueue = useCallback(async () => {
    if (
      isProcessingRef.current ||
      commandQueueRef.current.length === 0 ||
      !controlCharacteristic
    ) {
      return;
    }

    isProcessingRef.current = true;

    while (commandQueueRef.current.length > 0) {
      const controls = commandQueueRef.current.pop()!;
      commandQueueRef.current = [];

      if (
        lastSentControlsRef.current &&
        JSON.stringify(controls) === JSON.stringify(lastSentControlsRef.current)
      ) {
        console.log('Skipped duplicate:', controls.gripper);
        continue;
      }

      try {
        const jsonString = JSON.stringify(controls);
        const encoder = new TextEncoder();
        const data = encoder.encode(jsonString + '\n');

        await controlCharacteristic.writeValue(data);

        lastCommandTimeRef.current = Date.now();
        lastSentControlsRef.current = controls;

        console.log('✓ Sent:', controls.gripper);
      } catch (error) {
        console.error('Failed to send command:', error);
      }
    }

    isProcessingRef.current = false;
  }, [controlCharacteristic]);

  const sendCommand = useCallback(
    (controls: RobotControls) => {
      console.log(
        'sendCommand called, isConnected:',
        isConnected,
        'controls:',
        controls
      );

      if (!isConnected) {
        console.warn('Not connected to robot');
        return;
      }

      console.log('Adding to queue');
      commandQueueRef.current.push(controls);
      void processQueue();
    },
    [isConnected, processQueue]
  );

  return {
    sendCommand,
    isConnected,
  };
};
