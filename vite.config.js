import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  define: {
    // These will be replaced at build time
    __firebase_config: JSON.stringify(process.env.FIREBASE_CONFIG || '{"apiKey":"demo"}'),
    __app_id: JSON.stringify('candy-hft-pro-v2'),
    __initial_auth_token: JSON.stringify('')
  }
})
