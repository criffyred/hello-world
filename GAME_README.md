# 🍬 Candy HFT Pro - Retro Trading Arcade

A fast-paced, retro pixel-art trading simulator with arcade aesthetics! Trade candy futures, leverage positions, and compete for high scores!

## 💻 Platform Support

✅ **macOS** (Intel & Apple Silicon)
✅ **Windows**
✅ **Linux**

## 🎮 How to Play

### Quick Start (macOS/Linux/Windows)

```bash
# Install dependencies (if not already done)
npm install

# Start the development server
npm run dev
```

Then open your browser to `http://localhost:5173` to play!

### macOS Setup (First Time)

If you don't have Node.js installed:

```bash
# Using Homebrew (recommended)
brew install node

# Verify installation
node --version
npm --version

# Then run the game
npm install
npm run dev
```

## 🕹️ Game Instructions

### Objective
Make as much profit as possible over 20 trading days by buying/selling candy and using leveraged options!

### Controls & Gameplay

#### 1. **Start a Trading Day**
- Click the **"▶ START"** button when the market is offline
- Each trading session lasts **60 seconds**
- Choose your **location** for different volatility levels:
  - 🟢 **Playground**: Low risk, stable trends
  - 🟣 **Arcade**: Medium chaos, higher volatility
  - 🟠 **Skate Park**: Extreme risk/reward!

#### 2. **Trading Candies**
Four candy types to trade:
- 🐻 **GUMMY** - Low price range ($8-$30)
- 🍫 **CHOC** - Mid-high range ($40-$150)
- 🐛 **SOUR** - Medium range ($20-$80)
- 🍬 **GUM** - Wide range ($2-$200)

**Physical Trading:**
- **BUY** - Purchase 1 unit at current market price
- **SELL** - Sell 1 unit at current market price
- **ALL** - Liquidate your entire position instantly

#### 3. **Leverage Options (Advanced)**
Use high-frequency trading contracts for massive gains (or losses!):

- **10x** - Free starter leverage
- **100x** - Costs $5,000 to unlock
- **500x** - Costs $25,000 to unlock

**Option Types:**
- **UP** (Call) - Profit if price goes UP
- **DN** (Put) - Profit if price goes DOWN

⏰ Options expire in **10 seconds** - fast decisions required!

#### 4. **Watch the Market**
- 📊 **Charts** show real-time candlestick data
- 📰 **News ticker** at bottom affects prices
- 💰 Track your **Cash**, **Equity**, and positions in real-time

#### 5. **End Game**
- After **20 days**, final profit is calculated
- **Profit = Total Equity - Debt**
- Submit your score to the leaderboard!

### 💡 Pro Tips

1. **Watch the news** - Headlines impact specific candies or the whole market
2. **Manage risk** - Don't put all your cash in one trade
3. **Use leverage wisely** - 500x can make you rich... or broke
4. **Fast decisions** - Market moves every second!
5. **Location matters** - Higher volatility = higher risk/reward

### 🎯 Scoring Strategy

- Start with $1,500 cash and $2,000 debt
- Debt increases 4% per day
- Your goal: Maximize equity while managing debt
- Best players can turn thousands into millions!

## 🔧 Firebase Setup (Optional)

The game uses Firebase for leaderboards. To enable:

1. Create a Firebase project at https://console.firebase.google.com/
2. Enable Anonymous Authentication
3. Create a Firestore database
4. Copy your config and create a `.env` file:

```bash
cp .env.example .env
# Edit .env and add your Firebase config
```

**Note:** The game works without Firebase, you just won't have leaderboards!

## 🎨 Retro Features

- **Press Start 2P** pixel font
- CRT scanline effects
- RGB chromatic aberration
- Arcade-style chunky buttons
- Neon glow effects
- Pixel-perfect borders
- Retro pulse animations

## 🚀 Build for Production

```bash
npm run build
npm run preview
```

## 🎪 Have Fun!

This is a fast-paced arcade game - trades happen in seconds, fortunes change instantly. Can you beat the market? 🚀💰

---

**Controls:** Mouse/Click only
**Best played:** Fullscreen for maximum immersion
**Difficulty:** Easy to learn, hard to master!
