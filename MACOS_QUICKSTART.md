# 🍎 macOS Quick Start Guide

Get Candy HFT Pro running on your Mac in 5 minutes!

## ✅ Prerequisites

### Install Node.js (if you haven't already)

**Option 1: Using Homebrew (Recommended)**
```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Node.js
brew install node

# Verify installation
node --version  # Should show v18+ or higher
npm --version   # Should show v9+ or higher
```

**Option 2: Download from nodejs.org**
- Visit https://nodejs.org/
- Download the macOS installer (LTS version)
- Run the installer
- Restart Terminal

## 🚀 Run the Game

```bash
# 1. Navigate to the project directory
cd /path/to/hello-world

# 2. Install dependencies (first time only)
npm install

# 3. Start the development server
npm run dev
```

You should see:
```
  VITE v5.x.x  ready in xxx ms

  ➜  Local:   http://localhost:5173/
  ➜  Network: use --host to expose
```

## 🎮 Play the Game

1. Open your browser (Chrome, Safari, Firefox, or Edge)
2. Go to: **http://localhost:5173**
3. Click **"▶ START"** to begin trading!

## 🛑 Stop the Game

Press `Ctrl + C` in the Terminal to stop the server

## 🔧 Troubleshooting

### Port Already in Use
If port 5173 is taken:
```bash
# Kill the process using the port
lsof -ti:5173 | xargs kill -9

# Or use a different port
npm run dev -- --port 3000
```

### Permission Errors
```bash
# Fix npm permissions
sudo chown -R $(whoami) ~/.npm
sudo chown -R $(whoami) /usr/local/lib/node_modules
```

### Firewall Warnings
- macOS may ask for network access permission
- Click "Allow" when prompted

### Browser Issues
- Use Chrome or Firefox for best compatibility
- Make sure JavaScript is enabled
- Try clearing cache (Cmd + Shift + R)

## 🎯 macOS-Specific Tips

### Better Performance
- **Apple Silicon (M1/M2/M3)**: Everything runs natively and fast!
- **Intel Macs**: Fully supported, might be slightly slower

### Recommended Setup
- Use **iTerm2** or default Terminal app
- Keep the Terminal window open while playing
- Use fullscreen mode in browser for best experience

### Keyboard Shortcuts
- `Cmd + Q` - Quit Terminal (stops the server)
- `Cmd + T` - New Terminal tab (keep game running)
- `Cmd + R` - Refresh browser (reload game)

## 🔥 Quick Commands Cheat Sheet

```bash
# Start game
npm run dev

# Build for production
npm run build

# Preview production build
npm run preview

# Install new dependencies
npm install

# Clear cache and reinstall
rm -rf node_modules package-lock.json && npm install
```

## 🎨 Optional: Dark Mode Terminal

Make it match the retro aesthetic:
1. Open Terminal Preferences (Cmd + ,)
2. Go to Profiles
3. Select "Homebrew" or "Pro" theme
4. Set opacity to 90% for that retro CRT feel

## 🍬 Ready to Trade!

Your retro trading arcade is ready! Make those candy trades and climb the leaderboard! 🚀💰

---

**Need Help?** Check GAME_README.md for gameplay instructions
