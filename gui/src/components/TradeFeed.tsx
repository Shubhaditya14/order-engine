import React from 'react';
import type { Trade } from '../App';
import './TradeFeed.css';

interface Props {
    trades: Trade[];
}

const TradeFeed: React.FC<Props> = ({ trades }) => {
    return (
        <div className="trade-feed">
            <h3>Market Trades</h3>
            <div className="trade-list">
                {trades.map((trade, i) => (
                    <div key={i} className="trade-item">
                        <span className="price">{trade.price.toFixed(2)}</span>
                        <span className="qty">{trade.qty}</span>
                        <span className="time">{new Date().toLocaleTimeString()}</span>
                    </div>
                ))}
            </div>
        </div>
    );
};

export default TradeFeed;
