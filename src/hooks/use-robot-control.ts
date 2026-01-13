import { useCallback, useRef, useEffect } from 'react';
import type { UseRobotControlOptions, RobotControls } from '../types';
import { useConnection } from './use-connection';

export const useRobotControl = (options?: UseRobotControlOptions) => {
  const { heartbeatInterval = 2000 } = options || {};
  
  // Hole Connection-State aus dem Context!
  const { isConnected, controlCharacteristic, checkConnection } = useConnection();
  
  const heartbeatTimerRef = useRef<number>(0);
  const lastCommandTimeRef = useRef<number>(0);
  const lastSentControlsRef = useRef<RobotControls | null>(null);
  const commandQueueRef = useRef<RobotControls[]>([]);
  const isProcessingRef = useRef(false);

  const resetHeartbeatRef = useRef<() => void | undefined>(undefined);

  const resetHeartbeat = useCallback(() => {
    if (heartbeatTimerRef.current) {
      clearTimeout(heartbeatTimerRef.current);
    }

    heartbeatTimerRef.current = setTimeout(async () => {
      const timeSinceLastCommand = Date.now() - lastCommandTimeRef.current;
      
      if (timeSinceLastCommand >= heartbeatInterval) {
        await checkConnection();
      }
      
      resetHeartbeatRef.current?.();
    }, heartbeatInterval) as unknown as number;
  }, [heartbeatInterval, checkConnection]);

  useEffect(() => {
    resetHeartbeatRef.current = resetHeartbeat;
  }, [resetHeartbeat]);

  const processQueue = useCallback(async () => {
    if (isProcessingRef.current || commandQueueRef.current.length === 0 || !controlCharacteristic) {
      return;
    }

    isProcessingRef.current = true;

    while (commandQueueRef.current.length > 0) {
      const controls = commandQueueRef.current.pop()!;
      commandQueueRef.current = [];

      if (lastSentControlsRef.current && 
          JSON.stringify(controls) === JSON.stringify(lastSentControlsRef.current)) {
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
        resetHeartbeat();
        
        console.log('✓ Sent:', controls.gripper);
      } catch (error) {
        console.error('Failed to send command:', error);
      }
    }

    isProcessingRef.current = false;
  }, [controlCharacteristic, resetHeartbeat]);

  const sendCommand = useCallback((controls: RobotControls) => {
    console.log('sendCommand called, isConnected:', isConnected, 'controls:', controls);
    
    if (!isConnected) {
      console.warn('Not connected to robot');
      return;
    }
    
    console.log('Adding to queue');
    commandQueueRef.current.push(controls);
    processQueue();
  }, [isConnected, processQueue]);

  useEffect(() => {
    if (isConnected) {
      resetHeartbeat();
    }
    return () => {
      if (heartbeatTimerRef.current) {
        clearTimeout(heartbeatTimerRef.current);
      }
    };
  }, [isConnected, resetHeartbeat]);

  return {
    sendCommand,
    isConnected,
  };
};
