import './app.css'
import ControlScreen from './control-screen';
import ConnectionScreen from './connection-screen';
import { useRobotControl } from './hooks/use-robot-control';

function App() {
  const { isConnected} = useRobotControl();

  return isConnected ? <ControlScreen /> : <ConnectionScreen />
}

export default App
