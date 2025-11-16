import { useCallback, useRef, useEffect } from 'react';
import { useConnection } from './use-connection';
import type { UseRobotControlOptions, RobotControls } from '../types';

const ROBOT_BASE_URL = 'http://192.168.4.1';

export const useRobotControl = (options?: UseRobotControlOptions) => {
  const { heartbeatInterval = 2000 } = options || {};
  const heartbeatTimerRef = useRef<number>(0);
  const lastCommandTimeRef = useRef<number>(0);
  const lastSentControlsRef = useRef<RobotControls | null>(null);
  const commandQueueRef = useRef<RobotControls[]>([]);
  const isProcessingRef = useRef(false);

  const { isConnected, checkConnection } = useConnection();

  const resetHeartbeat = useCallback(() => {
    if (heartbeatTimerRef.current) {
      clearTimeout(heartbeatTimerRef.current);
    }

    heartbeatTimerRef.current = setTimeout(async () => {
      const timeSinceLastCommand = Date.now() - lastCommandTimeRef.current;
      
      if (timeSinceLastCommand >= heartbeatInterval) {
        await checkConnection();
      }
      
      resetHeartbeat();
    }, heartbeatInterval);
  }, [heartbeatInterval, checkConnection]);

  const processQueue = useCallback(async () => {
    if (isProcessingRef.current || commandQueueRef.current.length === 0) {
      return;
    }

    isProcessingRef.current = true;

    while (commandQueueRef.current.length > 0) {
      const controls = commandQueueRef.current.pop()!;
      commandQueueRef.current = [];

      // Skip if same as last sent value
      if (lastSentControlsRef.current && 
          JSON.stringify(controls) === JSON.stringify(lastSentControlsRef.current)) {
        console.log('Skipped duplicate:', controls.gripper);
        continue;
      }

      try {
        const response = await fetch(`${ROBOT_BASE_URL}/control`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(controls),
        });

        if (!response.ok) {
          throw new Error('Command failed');
        }

        lastCommandTimeRef.current = Date.now();
        lastSentControlsRef.current = controls;
        resetHeartbeat();
        
        console.log('Sent:', controls.gripper);
      } catch (error) {
        console.error('Failed to send command:', error);
      }
    }

    isProcessingRef.current = false;
  }, [resetHeartbeat]);

  const sendCommand = useCallback((controls: RobotControls) => {
    commandQueueRef.current.push(controls);
    processQueue();
  }, [processQueue]);

  useEffect(() => {
    resetHeartbeat();
    return () => {
      if (heartbeatTimerRef.current) {
        clearTimeout(heartbeatTimerRef.current);
      }
    };
  }, [resetHeartbeat]);

  return {
    sendCommand,
    isConnected,
  };
};
