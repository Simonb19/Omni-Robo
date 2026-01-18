import { useState, useEffect, useCallback, useRef } from 'react';
import ConnectionContext from '../contexts/connection-context';
import {
  CONTROL_CHARACTERISTIC_UUID,
  HEARTBEAT_TIMEOUT,
  SERVICE_UUID,
  STATUS_CHARACTERISTIC_UUID,
} from '@/constants';

function ConnectionContextProvider({ children }: React.PropsWithChildren) {
  const [isConnected, setIsConnected] = useState<boolean>(false);
  const [device, setDevice] = useState<BluetoothDevice | null>(null);
  const [controlCharacteristic, setControlCharacteristic] =
    useState<BluetoothRemoteGATTCharacteristic | null>(null);
  const lastHeartbeatRef = useRef<number>(0);

  // const checkConnection = useCallback(async () => {
  //     if (!device || !device.gatt?.connected) {
  //         setIsConnected(false);
  //         return false;
  //     }
  //     setIsConnected(true);
  //     return true;
  // }, [device]);

  const connect = useCallback(async () => {
    try {
      // Request Bluetooth device
      const bluetoothDevice = await navigator.bluetooth.requestDevice({
        filters: [{ name: 'Omni Robo' }],
        optionalServices: [SERVICE_UUID],
      });

      console.log('Device selected:', bluetoothDevice.name);
      setDevice(bluetoothDevice);

      // Connect to GATT server
      const server = await bluetoothDevice.gatt!.connect();
      console.log('Connected to GATT server');

      // Get service
      const service = await server.getPrimaryService(SERVICE_UUID);

      // Get characteristics
      const controlChar = await service.getCharacteristic(
        CONTROL_CHARACTERISTIC_UUID
      );
      setControlCharacteristic(controlChar);

      const statusChar = await service.getCharacteristic(
        STATUS_CHARACTERISTIC_UUID
      );
      await statusChar.startNotifications();
      statusChar.addEventListener('characteristicvaluechanged', () => {
        lastHeartbeatRef.current = Date.now();
      });

      setIsConnected(true);
      console.log('Bluetooth connected!');

      // Handle disconnection
      bluetoothDevice.addEventListener('gattserverdisconnected', () => {
        console.log('Bluetooth disconnected');
        setIsConnected(false);
        setControlCharacteristic(null);
      });

      return true;
    } catch (error) {
      console.error('Bluetooth connection failed:', error);
      setIsConnected(false);
      return false;
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

  useEffect(() => {
    if (!isConnected) return;

    const interval = setInterval(() => {
      if (
        lastHeartbeatRef.current > 0 &&
        Date.now() - lastHeartbeatRef.current > HEARTBEAT_TIMEOUT
      ) {
        console.log('Heartbeat timeout - disconnecting');
        setIsConnected(false);
        device?.gatt?.disconnect();
      }
    }, 500);

    return () => clearInterval(interval);
  }, [isConnected, device]);

  return (
    <ConnectionContext.Provider
      value={{
        isConnected,
        // checkConnection,
        connect,
        disconnect,
        device,
        controlCharacteristic,
      }}
    >
      {children}
    </ConnectionContext.Provider>
  );
}

export default ConnectionContextProvider;
