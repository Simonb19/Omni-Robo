import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import './index.css';
import App from './app.tsx';
import DebugScreen from './debug-screen';
import ConnectionContextProvider from './providers/connection-provider.tsx';
import { RouterProvider, createBrowserRouter } from 'react-router-dom';

const router = createBrowserRouter(
  [
    { path: '/', element: <App /> },
    { path: '/debug', element: <DebugScreen /> },
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
