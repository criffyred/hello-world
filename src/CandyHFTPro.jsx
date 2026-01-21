import React, { useState, useEffect, useMemo, useRef } from 'react';
import {
  TrendingUp, MapPin, X, Clock, Zap, BarChart3,
  Trophy, Play, ChevronUp, ChevronDown, AlertCircle, Newspaper,
  Lock, Unlock, ShoppingCart, Globe, Briefcase, DollarSign, Trash2,
  ArrowUpRight, ArrowDownRight
} from 'lucide-react';
import { initializeApp } from 'firebase/app';
import {
  getFirestore, collection, addDoc, onSnapshot, query, doc, setDoc
} from 'firebase/firestore';
import { getAuth, signInAnonymously, signInWithCustomToken, onAuthStateChanged } from 'firebase/auth';

// --- Firebase Setup ---
const firebaseConfig = JSON.parse(__firebase_config);
const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const db = getFirestore(app);
const appId = typeof __app_id !== 'undefined' ? __app_id : 'candy-hft-pro-v2';

// --- Constants ---
const SESSION_DURATION = 60;
const CANDLE_INTERVAL = 5;
const MAX_DAYS = 20;
const INITIAL_DEBT = 2000;
const INITIAL_CASH = 1500;
const OPTION_EXPIRY_TIME = 10; // 10 seconds as requested

const LOCATIONS = [
  { id: 'playground', name: 'Playground', vol: 0.05, bg: 'from-green-900/20 to-black', desc: 'Low noise, stable trends.' },
  { id: 'arcade', name: 'The Arcade', vol: 0.15, bg: 'from-purple-900/20 to-black', desc: 'High frequency chaos.' },
  { id: 'skatepark', name: 'Skate Park', vol: 0.35, bg: 'from-orange-900/20 to-black', desc: 'Extreme risk/reward.' },
];

const CANDIES = [
  { id: 'gummy', name: 'GUMMY', min: 8, max: 30, icon: '🐻', color: '#22c55e' },
  { id: 'choc', name: 'CHOC', min: 40, max: 150, icon: '🍫', color: '#eab308' },
  { id: 'sour', name: 'SOUR', min: 20, max: 80, icon: '🐛', color: '#a3ff00' },
  { id: 'gum', name: 'GUM', min: 2, max: 200, icon: '🍬', color: '#00d1ff' },
];

const LEVERAGE_LEVELS = [
  { id: 'tier1', label: '10x', multiplier: 10, cost: 0 },
  { id: 'tier2', label: '100x', multiplier: 100, cost: 5000 },
  { id: 'tier3', label: '500x', multiplier: 500, cost: 25000 },
];

const HEADLINES = [
  { text: "DENTISTS REPORT SURGE IN CAVITIES", impact: -0.15, target: "all" },
  { text: "TIKTOK CHALLENGE SPIKES GUMMY DEMAND", impact: 0.3, target: "gummy" },
  { text: "COCOA HARVEST DESTROYED BY RAIN", impact: 0.4, target: "choc" },
  { text: "NEW SYNTHETIC SOUR ACID DISCOVERED", impact: 0.2, target: "sour" },
  { text: "INFLATION HITS CONFECTIONERY SECTOR", impact: -0.1, target: "all" },
  { text: "CELEBRITY SPOTTED WITH MEGA LOLLIPOP", impact: 0.25, target: "gum" },
];

const formatMoney = (val) => `$${Number(val).toLocaleString(undefined, {minimumFractionDigits: 2, maximumFractionDigits: 2})}`;

// --- Components ---

const CandleChart = ({ candles }) => {
  if (!candles || candles.length === 0) return <div className="h-full w-full bg-slate-900/40" />;
  const allPrices = candles.flatMap(c => [c.high, c.low]);
  const min = Math.min(...allPrices) * 0.98;
  const max = Math.max(...allPrices) * 1.02;
  const range = max - min || 1;
  const chartWidth = 100;
  const candleWidth = (chartWidth / 12) * 0.7;

  return (
    <svg viewBox="0 0 100 100" className="w-full h-full pixel-render" preserveAspectRatio="none">
      {candles.map((c, i) => {
        const x = (i / 12) * 100;
        const isBull = c.close >= c.open;
        const col = isBull ? '#22c55e' : '#ef4444';
        const yHigh = 100 - ((c.high - min) / range) * 100;
        const yLow = 100 - ((c.low - min) / range) * 100;
        const yOpen = 100 - ((c.open - min) / range) * 100;
        const yClose = 100 - ((c.close - min) / range) * 100;
        return (
          <g key={i}>
            <line x1={x + candleWidth/2} y1={yHigh} x2={x + candleWidth/2} y2={yLow} stroke={col} strokeWidth="1.5" />
            <rect x={x} y={Math.min(yOpen, yClose)} width={candleWidth} height={Math.max(Math.abs(yOpen-yClose), 2)} fill={col} />
          </g>
        );
      })}
    </svg>
  );
};

export default function CandyHFTPro() {
  const [user, setUser] = useState(null);
  const [day, setDay] = useState(1);
  const [cash, setCash] = useState(INITIAL_CASH);
  const [debt, setDebt] = useState(INITIAL_DEBT);
  const [inventory, setInventory] = useState({});
  const [isMarketOpen, setIsMarketOpen] = useState(false);
  const [timeLeft, setTimeLeft] = useState(SESSION_DURATION);
  const [prices, setPrices] = useState({});
  const [candleData, setCandleData] = useState({});
  const [currentCandle, setCurrentCandle] = useState({});
  const [momentum, setMomentum] = useState({});
  const [options, setOptions] = useState([]);
  const [news, setNews] = useState("LOCATE OPPORTUNITY. EXECUTE.");
  const [gameOver, setGameOver] = useState(false);
  const [playerName, setPlayerName] = useState("");
  const [leaderboard, setLeaderboard] = useState([]);
  const [currentLoc, setCurrentLoc] = useState(LOCATIONS[0]);
  const [unlockedLeverage, setUnlockedLeverage] = useState(['tier1']);

  const tickRef = useRef(null);
  const pricesRef = useRef({}); // Ref to access current prices inside intervals/async without stale closure issues

  // Sync ref with state
  useEffect(() => {
    pricesRef.current = prices;
  }, [prices]);

  // Auth & Leaderboard
  useEffect(() => {
    const initAuth = async () => {
      if (typeof __initial_auth_token !== 'undefined' && __initial_auth_token) {
        await signInWithCustomToken(auth, __initial_auth_token);
      } else {
        await signInAnonymously(auth);
      }
    };
    initAuth();
    onAuthStateChanged(auth, setUser);
  }, []);

  useEffect(() => {
    if (!user) return;
    return onSnapshot(collection(db, 'artifacts', appId, 'public', 'data', 'scores'), (snap) => {
      const s = snap.docs.map(d => d.data());
      setLeaderboard(s.sort((a, b) => b.score - a.score).slice(0, 8));
    });
  }, [user]);

  // Initial setup
  useEffect(() => {
    const p = {}; const c = {}; const m = {}; const inv = {};
    CANDIES.forEach(x => {
      const startPrice = (x.min + x.max) / 2;
      p[x.id] = startPrice;
      c[x.id] = [];
      m[x.id] = 0;
      inv[x.id] = { quantity: 0, avgPrice: 0 };
    });
    setPrices(p); setCandleData(c); setMomentum(m); setInventory(inv);
  }, []);

  const startDay = () => {
    setIsMarketOpen(true);
    setTimeLeft(SESSION_DURATION);
    const startState = {};
    CANDIES.forEach(c => {
      startState[c.id] = { open: prices[c.id], high: prices[c.id], low: prices[c.id], close: prices[c.id] };
    });
    setCurrentCandle(startState);
    if (Math.random() > 0.4) {
      const h = HEADLINES[Math.floor(Math.random() * HEADLINES.length)];
      setNews(h.text);
      setMomentum(prev => {
        const next = {...prev};
        CANDIES.forEach(c => {
          if (h.target === 'all' || h.target === c.id) next[c.id] += h.impact;
        });
        return next;
      });
    }
  };

  useEffect(() => {
    if (isMarketOpen && timeLeft > 0) {
      tickRef.current = setInterval(() => {
        setTimeLeft(t => t - 1);
        setPrices(prev => {
          const next = { ...prev };
          const nextCandle = { ...currentCandle };
          const nextMoms = { ...momentum };

          CANDIES.forEach(c => {
            const noise = (Math.random() - 0.5) * currentLoc.vol;
            nextMoms[c.id] = (nextMoms[c.id] * 0.85) + noise;

            let move = 1 + (nextMoms[c.id] * 0.08);
            next[c.id] = Math.max(1, prev[c.id] * move);

            const cur = nextCandle[c.id];
            nextCandle[c.id] = {
              open: cur.open,
              high: Math.max(cur.high, next[c.id]),
              low: Math.min(cur.low, next[c.id]),
              close: next[c.id]
            };
          });

          if (timeLeft % CANDLE_INTERVAL === 0 && timeLeft !== SESSION_DURATION) {
            setCandleData(prevHist => {
              const h = { ...prevHist };
              CANDIES.forEach(c => {
                h[c.id] = [...(h[c.id] || []), nextCandle[c.id]].slice(-12);
                nextCandle[c.id] = { open: next[c.id], high: next[c.id], low: next[c.id], close: next[c.id] };
              });
              return h;
            });
          }

          setMomentum(nextMoms);
          setCurrentCandle(nextCandle);
          return next;
        });

        setOptions(prev => {
          const active = [];
          prev.forEach(o => {
            if (o.expiry <= 1) {
              const currentP = pricesRef.current[o.candyId] || 0;
              const diff = o.type === 'call' ? currentP - o.strike : o.strike - currentP;
              const payout = diff * o.amount;
              if (payout > 0) setCash(c => c + payout);
            } else {
              active.push({ ...o, expiry: o.expiry - 1 });
            }
          });
          return active;
        });

      }, 1000);
    } else if (timeLeft === 0 && isMarketOpen) {
      clearInterval(tickRef.current);
      setIsMarketOpen(false);
      setDay(d => d + 1);
      setDebt(d => d * 1.04);
      if (day >= MAX_DAYS) setGameOver(true);
    }
    return () => clearInterval(tickRef.current);
  }, [isMarketOpen, timeLeft, currentCandle, momentum, currentLoc]);

  const buyPhysical = (id) => {
    const currentPrice = pricesRef.current[id];
    if (cash >= currentPrice) {
      setCash(prev => prev - currentPrice);
      setInventory(prev => {
        const item = prev[id] || { quantity: 0, avgPrice: 0 };
        const totalQty = item.quantity + 1;
        const totalCost = (item.quantity * item.avgPrice) + currentPrice;
        return {
          ...prev,
          [id]: { quantity: totalQty, avgPrice: totalCost / totalQty }
        };
      });
    }
  };

  const sellPhysical = (id) => {
    const currentPrice = pricesRef.current[id];
    setInventory(prev => {
      const item = prev[id];
      if (item && item.quantity > 0) {
        setCash(c => c + currentPrice);
        return {
          ...prev,
          [id]: { ...item, quantity: item.quantity - 1 }
        };
      }
      return prev;
    });
  };

  const sellAllPhysical = (id) => {
    const currentPrice = pricesRef.current[id];
    setInventory(prev => {
      const item = prev[id];
      if (item && item.quantity > 0) {
        setCash(c => c + (currentPrice * item.quantity));
        return {
          ...prev,
          [id]: { quantity: 0, avgPrice: 0 }
        };
      }
      return prev;
    });
  };

  const openOption = (type, candyId, amount) => {
    const currentPrice = pricesRef.current[candyId];
    const premium = currentPrice * 0.1 * (amount / 10);
    if (cash < premium) return;
    setCash(c => c - premium);
    setOptions(o => [...o, {
      id: Date.now() + Math.random(),
      type,
      candyId,
      amount,
      strike: currentPrice,
      expiry: OPTION_EXPIRY_TIME
    }]);
  };

  const buyLicense = (tierId, cost) => {
    if (cash >= cost && !unlockedLeverage.includes(tierId)) {
      setCash(c => c - cost);
      setUnlockedLeverage(prev => [...prev, tierId]);
    }
  };

  // Total Equity calculation
  const totalEquity = useMemo(() => {
    let equity = cash;
    Object.keys(inventory).forEach(id => {
      equity += (inventory[id].quantity * (prices[id] || 0));
    });
    return equity;
  }, [cash, inventory, prices]);

  return (
    <div className={`min-h-screen bg-[#0a0014] text-[#00ff41] pixel-font p-3 flex flex-col gap-3 select-none transition-all duration-1000 bg-gradient-to-b ${currentLoc.bg}`}>

      <style>
        {`
          @import url('https://fonts.googleapis.com/css2?family=Press+Start+2P&display=swap');

          body {
            background: #000;
            overflow: hidden;
            margin: 0;
            padding: 0;
            image-rendering: pixelated;
            image-rendering: -moz-crisp-edges;
            image-rendering: crisp-edges;
          }

          .pixel-font {
            font-family: 'Press Start 2P', 'Courier New', monospace;
            line-height: 1.6;
          }

          .pixel-render {
            image-rendering: pixelated;
            image-rendering: -moz-crisp-edges;
            image-rendering: crisp-edges;
          }

          * { cursor: crosshair !important; }

          /* Enhanced CRT effect with scanlines */
          .crt-overlay::before {
            content: "";
            position: fixed;
            inset: 0;
            background:
              repeating-linear-gradient(
                0deg,
                rgba(0, 0, 0, 0.15),
                rgba(0, 0, 0, 0.15) 1px,
                transparent 1px,
                transparent 2px
              );
            pointer-events: none;
            z-index: 1001;
            animation: scanline 8s linear infinite;
          }

          /* RGB chromatic aberration */
          .crt-overlay::after {
            content: "";
            position: fixed;
            inset: 0;
            background:
              linear-gradient(rgba(18, 16, 16, 0) 50%, rgba(0, 0, 0, 0.15) 50%),
              linear-gradient(90deg, rgba(255, 0, 0, 0.03), rgba(0, 255, 0, 0.02), rgba(0, 0, 255, 0.03));
            background-size: 100% 4px, 4px 100%;
            pointer-events: none;
            z-index: 1000;
            mix-blend-mode: overlay;
          }

          @keyframes scanline {
            0% { transform: translateY(0); }
            100% { transform: translateY(10px); }
          }

          /* Pixel-perfect borders */
          .pixel-border {
            border-style: solid;
            border-image: repeating-linear-gradient(
              90deg,
              currentColor 0px,
              currentColor 2px,
              transparent 2px,
              transparent 4px
            ) 2;
          }

          /* Retro glow effects */
          .retro-glow {
            filter: drop-shadow(0 0 2px currentColor) drop-shadow(0 0 8px currentColor);
          }

          .retro-glow-strong {
            filter: drop-shadow(0 0 4px currentColor) drop-shadow(0 0 12px currentColor) drop-shadow(0 0 20px currentColor);
          }

          /* Chunky pixel shadows */
          .pixel-shadow {
            box-shadow:
              4px 4px 0px rgba(0, 0, 0, 0.8),
              8px 8px 0px rgba(0, 0, 0, 0.4);
          }

          .pixel-shadow-sm {
            box-shadow:
              2px 2px 0px rgba(0, 0, 0, 0.8),
              4px 4px 0px rgba(0, 0, 0, 0.4);
          }

          /* Arcade button effects */
          button {
            transition: all 0.05s ease;
            image-rendering: pixelated;
          }

          button:active {
            transform: translate(2px, 2px);
            box-shadow:
              2px 2px 0px rgba(0, 0, 0, 0.8) !important,
              4px 4px 0px rgba(0, 0, 0, 0.4) !important;
          }

          button:disabled {
            cursor: not-allowed !important;
            filter: grayscale(1) brightness(0.5);
            opacity: 0.4;
          }

          /* Pixel scrollbar */
          .custom-scrollbar::-webkit-scrollbar { width: 8px; }
          .custom-scrollbar::-webkit-scrollbar-track {
            background: #000;
            border: 2px solid #00ff41;
          }
          .custom-scrollbar::-webkit-scrollbar-thumb {
            background: #00ff41;
            border: 1px solid #000;
          }

          /* Blink animation for retro feel */
          @keyframes blink {
            0%, 49% { opacity: 1; }
            50%, 100% { opacity: 0; }
          }

          .blink {
            animation: blink 1s step-end infinite;
          }

          /* Retro pulse */
          @keyframes retro-pulse {
            0%, 100% {
              filter: brightness(1) drop-shadow(0 0 4px currentColor);
            }
            50% {
              filter: brightness(1.4) drop-shadow(0 0 12px currentColor);
            }
          }

          .retro-pulse {
            animation: retro-pulse 2s ease-in-out infinite;
          }

          /* Arcade screen flicker */
          @keyframes flicker {
            0% { opacity: 1; }
            2% { opacity: 0.8; }
            4% { opacity: 1; }
            8% { opacity: 0.9; }
            10% { opacity: 1; }
            100% { opacity: 1; }
          }

          .screen-flicker {
            animation: flicker 8s linear infinite;
          }
        `}
      </style>
      <div className="crt-overlay screen-flicker" />

      {/* Top HUD - Enhanced with pixel styling */}
      <div className="grid grid-cols-4 gap-3 border-4 border-[#00ff41] p-4 bg-black/95 backdrop-blur-md pixel-shadow relative z-20">
        <div className="border-r-4 border-[#00ff41]/30 pr-3">
          <div className="text-[8px] text-gray-500 uppercase flex items-center gap-1 mb-1">
            <Globe size={10} className="retro-glow"/> NETWORK
          </div>
          <div className="font-black flex items-center gap-2 text-[10px]">
            <div className={`w-3 h-3 border-2 border-black ${isMarketOpen ? 'bg-green-500 retro-pulse' : 'bg-red-500 blink'}`}
                 style={{boxShadow: isMarketOpen ? '0 0 8px #22c55e' : '0 0 8px #ef4444'}} />
            <span className="retro-glow">{isMarketOpen ? `DAY ${day} - ${currentLoc.name.toUpperCase()}` : 'OFFLINE'}</span>
          </div>
        </div>
        <div className="border-r-4 border-[#00ff41]/30 text-center pr-3">
          <div className="text-[8px] text-gray-500 uppercase mb-1">CASH</div>
          <div className="text-[12px] font-black text-[#00ffff] retro-glow">{formatMoney(cash)}</div>
        </div>
        <div className="border-r-4 border-[#00ff41]/30 text-center pr-3">
          <div className="text-[8px] text-gray-500 uppercase mb-1">EQUITY</div>
          <div className={`text-[12px] font-black ${totalEquity >= INITIAL_CASH ? 'text-[#00ff41] retro-glow' : 'text-[#ff0066] retro-glow'}`}>
            {formatMoney(totalEquity)}
          </div>
        </div>
        <div className="text-right">
          <div className="text-[8px] text-gray-500 uppercase mb-1">TIMER</div>
          <div className={`text-[14px] font-black tabular-nums ${timeLeft < 10 ? 'text-[#ff0066] retro-pulse' : 'text-white retro-glow'}`}>
            {timeLeft}s
          </div>
        </div>
      </div>

      {/* Main Terminal */}
      <div className="flex-grow grid grid-cols-12 gap-3 overflow-hidden relative z-20">
        <div className="col-span-8 flex flex-col gap-3 overflow-y-auto pr-1 custom-scrollbar">
          {CANDIES.map(c => (
            <div key={c.id} className="bg-black border-4 border-[#00ff41]/50 p-4 grid grid-cols-12 items-center gap-4 hover:border-[#00ffff] hover:shadow-[0_0_20px_rgba(0,255,255,0.3)] transition-all pixel-shadow-sm group">
              <div className="col-span-2 flex flex-col items-center">
                <span className="text-3xl mb-2 group-hover:scale-125 transition-transform pixel-render filter drop-shadow-[0_0_8px_rgba(255,255,255,0.5)]">
                  {c.icon}
                </span>
                <div className="text-white font-black text-[8px] tracking-widest retro-glow">{c.name}</div>
              </div>

              <div className="col-span-3 h-24 bg-black border-4 border-[#00ff41]/20 pixel-render p-1 pointer-events-none pixel-shadow-sm">
                <CandleChart candles={[...(candleData[c.id] || []), currentCandle[c.id]].filter(Boolean)} />
              </div>

              <div className="col-span-3 text-center flex flex-col gap-2 px-2">
                <div className="text-[9px] text-[#00ffff] font-bold uppercase tabular-nums bg-[#00ffff]/10 border-2 border-[#00ffff]/40 py-1 retro-glow">
                  ${prices[c.id]?.toFixed(2)}
                </div>

                <button
                  disabled={!isMarketOpen || cash < (prices[c.id] || 0)}
                  onClick={(e) => { e.stopPropagation(); buyPhysical(c.id); }}
                  className="w-full bg-green-500 border-4 border-green-300 text-[8px] py-2 font-black text-black hover:bg-green-300 hover:border-green-500 transition-all uppercase pixel-shadow-sm retro-glow"
                >
                  BUY
                </button>

                <div className="grid grid-cols-2 gap-2">
                  <button
                    disabled={!isMarketOpen || !inventory[c.id]?.quantity}
                    onClick={(e) => { e.stopPropagation(); sellPhysical(c.id); }}
                    className="bg-red-500 border-4 border-red-300 text-[7px] py-1.5 font-black text-black hover:bg-red-300 hover:border-red-500 transition-all uppercase pixel-shadow-sm"
                  >
                    SELL
                  </button>
                  <button
                    disabled={!isMarketOpen || !inventory[c.id]?.quantity}
                    onClick={(e) => { e.stopPropagation(); sellAllPhysical(c.id); }}
                    className="bg-red-700 border-4 border-red-500 text-[7px] py-1.5 font-black text-white hover:bg-white hover:text-red-900 hover:border-white transition-all uppercase pixel-shadow-sm"
                  >
                    ALL
                  </button>
                </div>
              </div>

              <div className="col-span-4 grid grid-cols-3 gap-2 px-2 border-l-4 border-[#00ff41]/20 pl-3">
                {LEVERAGE_LEVELS.map(tier => {
                  const isUnlocked = unlockedLeverage.includes(tier.id);
                  return (
                    <div key={tier.id} className="flex flex-col gap-1.5">
                      <button
                        disabled={!isMarketOpen || !isUnlocked}
                        onClick={() => openOption('call', c.id, tier.multiplier)}
                        className={`text-[7px] font-black py-2 border-4 transition-all flex flex-col items-center justify-center pixel-shadow-sm
                          ${isUnlocked ? 'bg-green-500 border-green-300 text-black hover:bg-green-300 hover:border-green-500 retro-glow' : 'bg-gray-900 border-gray-700 text-gray-700'}`}
                      >
                        {isUnlocked ? <span className="retro-glow-strong">{tier.label}</span> : <Lock size={10}/>}
                        <span className="text-[6px] mt-0.5">UP</span>
                      </button>
                      <button
                        disabled={!isMarketOpen || !isUnlocked}
                        onClick={() => openOption('put', c.id, tier.multiplier)}
                        className={`text-[7px] font-black py-2 border-4 transition-all flex flex-col items-center justify-center pixel-shadow-sm
                          ${isUnlocked ? 'bg-red-500 border-red-300 text-black hover:bg-red-300 hover:border-red-500 retro-glow' : 'bg-gray-900 border-gray-700 text-gray-700'}`}
                      >
                        {isUnlocked ? <span className="retro-glow-strong">{tier.label}</span> : <Lock size={10}/>}
                        <span className="text-[6px] mt-0.5">DN</span>
                      </button>
                    </div>
                  );
                })}
              </div>
            </div>
          ))}
        </div>

        {/* Sidebar */}
        <div className="col-span-4 flex flex-col gap-3 overflow-hidden">

          <div className="bg-black/95 border-4 border-[#00ff41]/50 p-3 flex flex-col gap-2 flex-grow overflow-hidden pixel-shadow">
            <div className="text-[8px] text-[#00ff41] font-bold uppercase tracking-widest flex items-center justify-between border-b-4 border-[#00ff41]/30 pb-2">
              <span className="flex items-center gap-1 retro-glow">
                <Briefcase size={10}/> POSITIONS
              </span>
              <span className="text-[6px] text-gray-500 blink">◉ LIVE</span>
            </div>

            <div className="flex-grow overflow-y-auto custom-scrollbar space-y-2 py-2">
              {CANDIES.map(c => {
                const inv = inventory[c.id] || { quantity: 0, avgPrice: 0 };
                if (inv.quantity === 0) return null;
                const pnl = (prices[c.id] - inv.avgPrice) * inv.quantity;
                const pnlPct = ((prices[c.id] - inv.avgPrice) / inv.avgPrice) * 100;

                return (
                  <div key={c.id} className="bg-[#001100] border-2 border-[#00ff41]/30 p-2 flex flex-col gap-1.5 pixel-shadow-sm hover:border-[#00ff41] transition-all">
                    <div className="flex justify-between items-center">
                      <div className="flex items-center gap-2">
                        <span className="text-lg pixel-render">{c.icon}</span>
                        <span className="text-[8px] font-black text-white">{c.name}</span>
                        <span className="text-[7px] bg-[#00ff41] text-black px-1.5 py-0.5 font-black border border-black">
                          x{inv.quantity}
                        </span>
                      </div>
                      <div className={`text-[8px] font-black tabular-nums ${pnl >= 0 ? 'text-green-400 retro-glow' : 'text-red-400 retro-glow'}`}>
                        {pnl >= 0 ? '+' : ''}{pnl.toFixed(2)}
                      </div>
                    </div>
                    <div className="flex justify-between items-center mt-1">
                      <div className="text-[6px] text-gray-500">BASIS: ${inv.avgPrice.toFixed(2)}</div>
                      <button
                        onClick={() => sellAllPhysical(c.id)}
                        className="bg-red-500 border-2 border-red-300 text-black text-[6px] px-2 py-1 font-black hover:bg-red-300 hover:border-red-500 transition-all pixel-shadow-sm"
                      >
                        DUMP
                      </button>
                    </div>
                  </div>
                );
              })}

              {options.length > 0 && (
                <div className="pt-2 border-t-2 border-[#00ff41]/20 mt-2">
                  <div className="text-[7px] text-gray-500 font-black mb-2 uppercase retro-glow">⚡ CONTRACTS</div>
                  {options.map(o => {
                    const diff = o.type === 'call' ? prices[o.candyId] - o.strike : o.strike - prices[o.candyId];
                    const curValue = diff * o.amount;
                    return (
                      <div key={o.id} className="flex justify-between items-center text-[8px] mb-2 p-1.5 bg-black border-2 border-[#00ff41]/30 pixel-shadow-sm">
                        <span className={`font-black uppercase ${o.type === 'call' ? 'text-green-400' : 'text-red-400'}`}>
                          {o.candyId} {o.type}
                        </span>
                        <span className={`font-black tabular-nums ${curValue > 0 ? 'text-green-400 retro-glow' : 'text-gray-600'}`}>
                          ${Math.max(0, curValue).toFixed(2)}
                        </span>
                        <span className="text-[7px] bg-[#00ff41] text-black px-1.5 py-0.5 flex items-center gap-1 font-black border border-black">
                          <Clock size={8}/> {o.expiry}
                        </span>
                      </div>
                    );
                  })}
                </div>
              )}

              {Object.values(inventory).every(i => i.quantity === 0) && options.length === 0 && (
                <div className="h-full flex flex-col items-center justify-center opacity-30 py-10">
                  <Briefcase size={24} className="mb-2 retro-glow"/>
                  <span className="text-[7px] font-bold blink">EMPTY</span>
                </div>
              )}
            </div>
          </div>

          <div className="bg-black border-4 border-[#00ff41]/50 p-3 flex flex-col gap-2 pixel-shadow">
            {!isMarketOpen ? (
              <>
                <button onClick={startDay} className="bg-[#00ff41] text-black font-black py-4 text-[10px] hover:bg-[#00ffff] hover:text-black border-4 border-[#00ff41] hover:border-[#00ffff] transition-all uppercase pixel-shadow retro-glow-strong">
                  ▶ START
                </button>
                <div className="grid grid-cols-3 gap-1.5">
                  {LOCATIONS.map(l => (
                    <button
                      key={l.id}
                      onClick={() => setCurrentLoc(l)}
                      className={`p-2 text-[6px] border-2 transition-all flex flex-col items-center font-black pixel-shadow-sm
                        ${currentLoc.id === l.id ? 'bg-[#00ff41]/20 border-[#00ff41] text-[#00ff41] retro-glow' : 'border-gray-700 text-gray-600 bg-black'}`}
                    >
                      <span className="uppercase">{l.name.split(' ')[0]}</span>
                    </button>
                  ))}
                </div>
                <div className="mt-1 space-y-1.5">
                  {LEVERAGE_LEVELS.slice(1).map(tier => (
                    <button
                      key={tier.id}
                      disabled={unlockedLeverage.includes(tier.id) || cash < tier.cost}
                      onClick={() => buyLicense(tier.id, tier.cost)}
                      className={`w-full p-2 text-left text-[7px] border-4 flex justify-between items-center transition-all font-black pixel-shadow-sm
                        ${unlockedLeverage.includes(tier.id) ? 'border-green-800 text-green-800 bg-green-900/20' : 'border-blue-500 text-black bg-blue-500 hover:bg-blue-300 hover:border-blue-300'}`}
                    >
                      <span className="uppercase flex items-center gap-1">
                        {unlockedLeverage.includes(tier.id) ? <Unlock size={8}/> : <ShoppingCart size={8}/>}
                        {tier.label}
                      </span>
                      <span className="font-black text-[6px]">{unlockedLeverage.includes(tier.id) ? '✓' : formatMoney(tier.cost)}</span>
                    </button>
                  ))}
                </div>
              </>
            ) : (
              <div className="flex flex-col items-center justify-center py-6 gap-2 border-4 border-[#ff0066]/50 bg-[#ff0066]/10">
                <div className="text-[10px] font-black text-[#ff0066] retro-pulse">⊗ LOCKED</div>
                <div className="text-[6px] text-gray-600 uppercase">SESSION ACTIVE</div>
              </div>
            )}
          </div>
        </div>
      </div>

      {/* Footer Ticker - Enhanced with retro feel */}
      <div className="bg-black border-4 border-[#00ff41] p-2 h-12 flex items-center overflow-hidden relative z-20 pixel-shadow">
         <div className="whitespace-nowrap flex items-center gap-8 text-[8px] font-black text-[#00ff41]">
           <div className="flex items-center gap-2 text-yellow-400 bg-yellow-500/20 px-3 py-1 border-2 border-yellow-500/40 retro-glow">
             <Newspaper size={12}/> NEWS:
           </div>
           <div className="animate-in fade-in slide-in-from-right duration-700 uppercase text-white retro-glow">{news}</div>
           <div className="opacity-50 text-[6px] tracking-widest">| HFT v2.1 | {user?.uid?.slice(0,8)} | PING: 12ms |</div>
         </div>
      </div>

      {/* End Game Modal - Retro arcade style */}
      {gameOver && (
        <div className="fixed inset-0 bg-black/98 z-[1001] flex items-center justify-center p-6 backdrop-blur-md">
          <div className="border-8 border-[#00ff41] p-12 bg-black max-w-md w-full text-center pixel-shadow retro-glow-strong">
             <h2 className="text-[24px] font-black mb-4 italic tracking-tighter text-[#00ff41] uppercase retro-pulse">
               GAME OVER
             </h2>
             <div className="text-[16px] font-black text-[#00ffff] mb-2 uppercase retro-glow">PROFIT</div>
             <div className="text-[32px] font-black text-white mb-8 tabular-nums retro-glow-strong border-4 border-[#00ff41]/30 py-4 bg-[#00ff41]/5">
               {formatMoney(totalEquity - debt)}
             </div>
             <div className="space-y-4">
               <input
                 type="text"
                 className="bg-[#001100] border-4 border-[#00ff41] p-4 w-full text-center text-[#00ff41] outline-none focus:border-[#00ffff] uppercase font-black tracking-widest text-[12px] pixel-shadow-sm retro-glow"
                 placeholder="NAME"
                 maxLength={10}
                 value={playerName}
                 onChange={e => setPlayerName(e.target.value.toUpperCase())}
               />
               <button
                onClick={async () => {
                  if (!playerName) return;
                  await addDoc(collection(db, 'artifacts', appId, 'public', 'data', 'scores'), {
                    name: playerName,
                    score: totalEquity - debt,
                    ts: Date.now()
                  });
                  window.location.reload();
                }}
                className="bg-[#00ff41] text-black font-black w-full py-5 hover:bg-[#00ffff] hover:text-black border-4 border-[#00ff41] hover:border-[#00ffff] transition-all text-[10px] uppercase pixel-shadow retro-glow-strong"
               >
                ▶ SUBMIT SCORE
               </button>
             </div>
          </div>
        </div>
      )}
    </div>
  );
}
