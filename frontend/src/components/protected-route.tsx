import { useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { useConnection } from '@/hooks/use-connection';

function ProtectedRoute({ children }: { children: React.ReactNode }) {
  const { isConnected } = useConnection();
  const navigate = useNavigate();

  useEffect(() => {
    console.log(isConnected);
    if (!isConnected) {
      navigate('/');
    }
  }, [isConnected, navigate]);

  return isConnected ? children : null;
}

export default ProtectedRoute;
