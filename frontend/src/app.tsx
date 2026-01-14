import './app.css';
import ControlScreen from './control-screen';
import ConnectionScreen from './connection-screen';
import { useConnection } from './hooks/use-connection';

function App() {
  const { isConnected } = useConnection();

  return isConnected ? <ControlScreen /> : <ConnectionScreen />;
}

export default App;
