import { useState, useEffect, useCallback, useRef } from "react";
import ConnectionContext from "../contexts/connection-context";

const SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
const CONTROL_CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';
//const STATUS_CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a9';

function ConnectionContextProvider({children}: React.PropsWithChildren) {
    const [isConnected, setIsConnected] = useState<boolean>(false);
    const [device, setDevice] = useState<BluetoothDevice | null>(null);
    const [controlCharacteristic, setControlCharacteristic] = useState<BluetoothRemoteGATTCharacteristic | null>(null);
    const checkIntervalRef = useRef<number | undefined>(undefined);

    const checkConnection = useCallback(async () => {
        if (!device || !device.gatt?.connected) {
            setIsConnected(false);
            return false;
        }
        setIsConnected(true);
        return true;
    }, [device]);

    const connect = useCallback(async () => {
        try {
            // Request Bluetooth device
            const bluetoothDevice = await navigator.bluetooth.requestDevice({
                filters: [{ name: 'Omni Robo' }],
                optionalServices: [SERVICE_UUID]
            });

            console.log('Device selected:', bluetoothDevice.name);
            setDevice(bluetoothDevice);

            // Connect to GATT server
            const server = await bluetoothDevice.gatt!.connect();
            console.log('Connected to GATT server');

            // Get service
            const service = await server.getPrimaryService(SERVICE_UUID);
            
            // Get characteristics
            const controlChar = await service.getCharacteristic(CONTROL_CHARACTERISTIC_UUID);
            setControlCharacteristic(controlChar);

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
        // Check connection periodically
        checkIntervalRef.current = setInterval(() => {
            checkConnection();
        }, 2000) as unknown as number;

        return () => {
            if (checkIntervalRef.current) {
                clearInterval(checkIntervalRef.current);
            }
            // Cleanup on unmount
            if (device?.gatt?.connected) {
                device.gatt.disconnect();
            }
        };
    }, [checkConnection, device]);

    return (
        <ConnectionContext.Provider 
            value={{
                isConnected, 
                checkConnection, 
                connect, 
                disconnect,
                device,
                controlCharacteristic
            }}
        >
            {children}
        </ConnectionContext.Provider>
    );
} 

export default ConnectionContextProvider;
