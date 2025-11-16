import { useConnection } from '../hooks/use-connection';

function ConnectButton() {
  const { isConnected, connect, disconnect } = useConnection();

  return (
    <div>
      <button 
        onClick={isConnected ? disconnect : connect}
        style={{
          padding: '10px 20px',
          fontSize: '16px',
          backgroundColor: isConnected ? '#dc3545' : '#007bff',
          color: 'white',
          border: 'none',
          borderRadius: '5px',
          cursor: 'pointer'
        }}
      >
        {isConnected ? '🔌 Trennen' : '📡 Verbinden'}
      </button>
      <p>Status: {isConnected ? '✅ Verbunden' : '❌ Getrennt'}</p>
    </div>
  );
}

export default ConnectButton;
