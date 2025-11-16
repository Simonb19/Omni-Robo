import { BounceLoader } from "react-spinners"
import { useConnection } from "./hooks/use-connection";
import './connection-screen.css'

function ConnectionScreen() {
  const { isConnected } = useConnection();
  
    return (
        <>
            <div className="container">
                <BounceLoader color="white"/>
                <p>{isConnected ? "Succesfully connected" : "Checking Connection..."}</p>
                <div className="border-1 p-4 rounded-2xl mt-4">
                <p>Name: Omni Robo</p>
                <p>Passwort: geheimnis</p>
                </div>
            </div>
        </>
    )
}

export default ConnectionScreen;