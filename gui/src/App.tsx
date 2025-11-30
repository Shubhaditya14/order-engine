import { useEffect, useState, useRef } from 'react';
import './App.css';
import OrderBook from './components/OrderBook';
import OrderEntry from './components/OrderEntry';
import TradeFeed from './components/TradeFeed';

export type Side = 'buy' | 'sell';

export interface Level {
  price: number;
  qty: number;
}

export interface Trade {
  price: number;
  qty: number;
  maker: number;
  taker: number;
  timestamp?: number;
}

function App() {
  const [bids, setBids] = useState<Level[]>([]);
  const [asks, setAsks] = useState<Level[]>([]);
  const [trades, setTrades] = useState<Trade[]>([]);
  const [connected, setConnected] = useState(false);
  const ws = useRef<WebSocket | null>(null);

  useEffect(() => {
    const connect = () => {
      const socket = new WebSocket('ws://localhost:8080');
      ws.current = socket;

      socket.onopen = () => {
        setConnected(true);
        console.log('Connected to OME');
      };

      socket.onclose = () => {
        setConnected(false);
        console.log('Disconnected');
        setTimeout(connect, 1000);
      };

      socket.onmessage = (event) => {
        const data = JSON.parse(event.data);
        if (data.type === 'snapshot' || data.type === 'book') {
          setBids(data.bids);
          setAsks(data.asks);
        } else if (data.type === 'trade') {
          setTrades(prev => [...data.trades, ...prev].slice(0, 50));
        }
      };
    };

    connect();

    return () => {
      ws.current?.close();
    };
  }, []);

  const sendOrder = (side: Side, price: number, qty: number) => {
    if (ws.current && ws.current.readyState === WebSocket.OPEN) {
      ws.current.send(JSON.stringify({
        type: 'add',
        side,
        price,
        qty
      }));
    }
  };

  const cancelOrder = (orderId: number) => {
    if (ws.current && ws.current.readyState === WebSocket.OPEN) {
      ws.current.send(JSON.stringify({
        type: 'cancel',
        orderId
      }));
    }
  }

  return (
    <div className="app-container">
      <header>
        <h1>ORDER MATCHING <span>ENGINE</span></h1>
        <div className={`status ${connected ? 'online' : 'offline'}`}>
          {connected ? 'SYSTEM ONLINE' : 'DISCONNECTED'}
        </div>
      </header>

      <main>
        <div className="left-panel">
          <OrderEntry onSendOrder={sendOrder} />
          <TradeFeed trades={trades} />
        </div>
        <div className="right-panel">
          <OrderBook bids={bids} asks={asks} />
        </div>
      </main>
    </div>
  );
}

export default App;
