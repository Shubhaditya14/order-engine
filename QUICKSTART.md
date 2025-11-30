# Quick Start Guide

## 🚀 Getting Started in 3 Steps

### Step 1: Build the System

```bash
./build.sh
```

This will:
- Configure and compile the C++ backend
- Install npm dependencies for the React GUI

### Step 2: Run the System

**Option A: Run Everything (Recommended)**
```bash
./build.sh run
```

**Option B: Run Separately**

Terminal 1 (Backend):
```bash
./build.sh backend
```

Terminal 2 (Frontend):
```bash
./build.sh frontend
```

### Step 3: Open Your Browser

Navigate to: **http://localhost:5173**

---

## 📊 Using the GUI

### Place Orders

1. **Select Side**: Click BUY or SELL
2. **Enter Price**: e.g., 100.00
3. **Enter Quantity**: e.g., 10
4. **Submit**: Click the BUY/SELL button

### Watch the Magic

- **Order Book** (right panel): See live bids and asks
- **Trade Feed** (bottom left): See executed trades in real-time
- **Connection Status** (top right): Verify you're connected

### Example Trading Session

```
1. Place BUY order: Price=100, Qty=50
2. Place BUY order: Price=99, Qty=100
3. Place SELL order: Price=101, Qty=30
4. Place SELL order: Price=100, Qty=25  ← This will MATCH!
   
   → Trade executes at price 100 for qty 25
   → Buy order at 100 now has 25 remaining
```

---

## 🧪 Testing via Browser Console

Open DevTools Console (F12) and try:

```javascript
// Get the WebSocket connection
const ws = new WebSocket('ws://localhost:8080');

ws.onopen = () => {
  console.log('Connected!');
  
  // Place a buy order
  ws.send(JSON.stringify({
    type: 'add',
    side: 'buy',
    price: 100,
    qty: 50
  }));
  
  // Place a sell order (will match!)
  ws.send(JSON.stringify({
    type: 'add',
    side: 'sell',
    price: 100,
    qty: 30
  }));
};

ws.onmessage = (event) => {
  console.log('Received:', JSON.parse(event.data));
};
```

---

## 🛠️ Build Commands

| Command | Description |
|---------|-------------|
| `./build.sh` | Build everything |
| `./build.sh clean` | Clean build artifacts |
| `./build.sh run` | Run backend + frontend |
| `./build.sh backend` | Run only backend |
| `./build.sh frontend` | Run only frontend |

---

## 🔧 Configuration

### Change WebSocket Port

**Backend** (`src/main.cpp`):
```cpp
ome::Server server(8080, engine);  // Change to your port
```

**Frontend** (`gui/src/App.tsx`):
```typescript
const socket = new WebSocket('ws://localhost:8080');  // Match backend port
```

Then rebuild:
```bash
./build.sh clean
./build.sh
```

---

## 📝 Project Structure

```
order-engine/
├── build.sh              ← Build script
├── CMakeLists.txt        ← C++ build config
├── README.md             ← Full documentation
├── ARCHITECTURE.md       ← Deep dive
├── QUICKSTART.md         ← This file
├── src/                  ← C++ source
│   ├── common/
│   ├── engine/
│   ├── server/
│   └── main.cpp
└── gui/                  ← React frontend
    ├── src/
    └── package.json
```

---

## ❓ Troubleshooting

### Backend won't compile

**Error**: `Could NOT find Boost`
- **Solution**: The project uses standalone Asio (no Boost needed). If you still see this, run `./build.sh clean` and rebuild.

**Error**: `C++20 not supported`
- **Solution**: Update your compiler (Clang 10+, GCC 10+)

### Frontend won't start

**Error**: `Cannot find module`
- **Solution**: Delete `gui/node_modules` and run `./build.sh` again

### WebSocket connection fails

**Error**: `WebSocket connection to 'ws://localhost:8080' failed`
- **Solution**: Make sure the backend is running (`./build.sh backend`)

### Port already in use

**Error**: `Address already in use`
- **Solution**: Kill the process using port 8080:
  ```bash
  lsof -ti:8080 | xargs kill -9
  ```

---

## 🎯 Next Steps

1. **Read the full README**: `README.md`
2. **Understand the architecture**: `ARCHITECTURE.md`
3. **Explore the code**: Start with `src/main.cpp`
4. **Customize the GUI**: Edit `gui/src/components/`
5. **Add features**: See "Future Enhancements" in README

---

## 🚨 Important Notes

- **This is a demo system**: Not production-ready
- **No persistence**: Orders are lost on restart
- **No authentication**: Anyone can connect
- **No risk checks**: Can place invalid orders
- **Single symbol**: Only one order book

For production use, you'd need:
- Persistence (database or log)
- Authentication & authorization
- Risk management
- Multi-symbol support
- FIX protocol
- Monitoring & alerting

---

**Happy Trading! 🚀**
