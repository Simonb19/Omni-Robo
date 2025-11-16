import { useCallback, useRef, useEffect, useState } from 'react';
import type { UseRobotControlOptions, RobotControls } from '../types';

const SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
const CONTROL_CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';
//const STATUS_CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a9';

export const useRobotControl = (options?: UseRobotControlOptions) => {
  const { heartbeatInterval = 2000 } = options || {};
  const [isConnected, setIsConnected] = useState(false);
  const [device, setDevice] = useState<BluetoothDevice | null>(null);
  const [controlCharacteristic, setControlCharacteristic] = useState<BluetoothRemoteGATTCharacteristic | null>(null);
  
  const heartbeatTimerRef = useRef<number>(0);
  const lastCommandTimeRef = useRef<number>(0);
  const lastSentControlsRef = useRef<RobotControls | null>(null);
  const commandQueueRef = useRef<RobotControls[]>([]);
  const isProcessingRef = useRef(false);

  const connect = useCallback(async () => {
    try {
      // Request Bluetooth device
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ name: 'Omni Robo' }],
        optionalServices: [SERVICE_UUID]
      });

      console.log('Device selected:', device.name);
      setDevice(device);

      // Connect to GATT server
      const server = await device.gatt!.connect();
      console.log('Connected to GATT server');

      // Get service
      const service = await server.getPrimaryService(SERVICE_UUID);
      
      // Get characteristics
      const controlChar = await service.getCharacteristic(CONTROL_CHARACTERISTIC_UUID);
      setControlCharacteristic(controlChar);

      setIsConnected(true);
      console.log('Bluetooth connected!');

      // Handle disconnection
      device.addEventListener('gattserverdisconnected', () => {
        console.log('Bluetooth disconnected');
        setIsConnected(false);
        setControlCharacteristic(null);
      });

    } catch (error) {
      console.error('Bluetooth connection failed:', error);
      setIsConnected(false);
    }
  }, []);

  const disconnect = useCallback(async () => {
    if (device?.gatt?.connected) {
      device.gatt.disconnect();
    }
    setDevice(null);
    setControlCharacteristic(null);
    setIsConnected(false);
  }, [device]);

  const checkConnection = useCallback(async () => {
    if (!device || !device.gatt?.connected) {
      setIsConnected(false);
      return false;
    }
    setIsConnected(true);
    return true;
  }, [device]);

  // Use ref to store the reset function to avoid circular dependency
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
      
      // Call via ref to avoid circular dependency
      resetHeartbeatRef.current?.();
    }, heartbeatInterval) as unknown as number;
  }, [heartbeatInterval, checkConnection]);

  // Update ref whenever resetHeartbeat changes
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

      // Skip if same as last sent value
      if (lastSentControlsRef.current && 
          JSON.stringify(controls) === JSON.stringify(lastSentControlsRef.current)) {
        console.log('Skipped duplicate:', controls.gripper);
        continue;
      }

      try {
        // Convert controls to JSON string and then to bytes
        const jsonString = JSON.stringify(controls);
        const encoder = new TextEncoder();
        const data = encoder.encode(jsonString + '\n');

        // Send via BLE characteristic
        await controlCharacteristic.writeValue(data);

        lastCommandTimeRef.current = Date.now();
        lastSentControlsRef.current = controls;
        resetHeartbeat();
        
        console.log('Sent:', controls.gripper);
      } catch (error) {
        console.error('Failed to send command:', error);
        setIsConnected(false);
      }
    }

    isProcessingRef.current = false;
  }, [controlCharacteristic, resetHeartbeat]);

  const sendCommand = useCallback((controls: RobotControls) => {
    if (!isConnected) {
      console.warn('Not connected to robot');
      return;
    }
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
    connect,
    disconnect,
  };
};
