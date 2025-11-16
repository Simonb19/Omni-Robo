import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import App from './app.tsx'
import ConnectionContextProvider from './providers/connection-provider.tsx'

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <ConnectionContextProvider> 
      <App />
    </ConnectionContextProvider>
  </StrictMode>,
)
