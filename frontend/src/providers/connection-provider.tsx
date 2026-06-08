// ============================================================
//  Verwaltet die Web-Bluetooth-Verbindung zum Roboter:
//  Verbinden, Trennen, Characteristics bereitstellen und über
//  einen Heartbeat erkennen, wenn die Verbindung abbricht.
//  Stellt alles per Context dem Rest der App zur Verfügung.
// ============================================================

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

    // Zeitpunkt der letzten Statusmeldung vom Roboter (für Heartbeat).
    const lastHeartbeatRef = useRef<number>(0);

    /**
     * Baut die komplette Verbindung auf: Gerät auswählen, mit dem
     * GATT-Server verbinden, Service + Characteristics holen und die
     * Status-Notifications abonnieren.
     * @return true bei Erfolg, sonst false
     */
    const connect = useCallback(async () => {
        try {
            // Gerät "Omni Robo" auswählen lassen.
            const bluetoothDevice = await navigator.bluetooth.requestDevice({
                filters: [{ name: 'Omni Robo' }],
                optionalServices: [SERVICE_UUID],
            });

            console.log('Device selected:', bluetoothDevice.name);
            setDevice(bluetoothDevice);

            // Mit dem GATT-Server verbinden.
            const server = await bluetoothDevice.gatt!.connect();
            console.log('Connected to GATT server');

            const service = await server.getPrimaryService(SERVICE_UUID);

            // Control-Characteristic: hierüber werden Befehle gesendet.
            const controlChar = await service.getCharacteristic(
                CONTROL_CHARACTERISTIC_UUID
            );
            setControlCharacteristic(controlChar);

            // Status-Characteristic: Notifications abonnieren -> jede
            // Meldung aktualisiert den Heartbeat-Zeitstempel.
            const statusChar = await service.getCharacteristic(
                STATUS_CHARACTERISTIC_UUID
            );
            await statusChar.startNotifications();
            statusChar.addEventListener('characteristicvaluechanged', () => {
                lastHeartbeatRef.current = Date.now();
            });

            setIsConnected(true);
            console.log('Bluetooth connected!');

            // Auf unerwarteten Verbindungsabbruch reagieren.
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

    /**
     * Trennt die Verbindung und setzt den Zustand zurück.
     */
    const disconnect = useCallback(async () => {
        if (device?.gatt?.connected) {
            device.gatt.disconnect();
        }
        setDevice(null);
        setControlCharacteristic(null);
        setIsConnected(false);
    }, [device]);

    // Heartbeat-Überwachung: kommt zu lange keine Statusmeldung,
    // gilt die Verbindung als verloren und wird getrennt.
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
