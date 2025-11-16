import { useState, useEffect } from "react";
import ConnectionContext from "../contexts/connection-context";

function ConnectionContextProvider ({children}: React.PropsWithChildren) {
    const [isConnected, setIsConnected] = useState<boolean>(false);

     const checkConnection = async () => {
        try {
            const response = await fetch('http://192.168.4.1/status');
            setIsConnected(response.ok)
            console.log(response)
        } catch {
            setIsConnected(false)
            console.log("Fetch was unsucessful!")
        }
    }

    useEffect(() => {
    const timeout = setTimeout(() => {
      checkConnection(); // initialer Check asynchron
    }, 0);

    const interval = setInterval(checkConnection, 2000);

    return () => {
      clearTimeout(timeout);
      clearInterval(interval);
    };
    }, [])

    return <ConnectionContext.Provider value={{isConnected, checkConnection}}>{children}</ConnectionContext.Provider>
} 

export default ConnectionContextProvider;