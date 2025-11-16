import { BounceLoader } from "react-spinners"
import { useConnection } from "./hooks/use-connection";
import ConnectButton from "./components/connection-button";
import './connection-screen.css'

function ConnectionScreen() {
  const { isConnected } = useConnection();
  
    return (
        <>
            <div className="container gap-4">
        {!isConnected ? (
            <>
            <h1>🤖 Omni Robo</h1>
            <p>Verbinde dich mit deinem Roboter</p>
            <ConnectButton />
            </>
        ) : (
            <>
            <h2>✅ Verbunden!</h2>
            <p>Roboter ist bereit</p>
            <ConnectButton />
            </>
        )}
        </div>
        </>
    )
}

export default ConnectionScreen;