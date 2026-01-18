import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import './index.css';
import App from './app.tsx';
import DebugScreen from './debug-screen';
import ConnectionContextProvider from './providers/connection-provider.tsx';
import { RouterProvider, createBrowserRouter } from 'react-router-dom';
import ProtectedRoute from '@/components/protected-route.tsx';

const router = createBrowserRouter(
  [
    { path: '/', element: <App /> },
    { path: '/debug', element: <ProtectedRoute><DebugScreen /></ProtectedRoute> },
  ],
  {
    basename: '/Omni-Robo',
  }
);

document.documentElement.classList.add('dark');

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <ConnectionContextProvider>
      <RouterProvider router={router} />
    </ConnectionContextProvider>
  </StrictMode>
);
