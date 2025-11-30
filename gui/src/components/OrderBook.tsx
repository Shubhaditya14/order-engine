import React from 'react';
import type { Level } from '../App';
import './OrderBook.css';

interface Props {
    bids: Level[];
    asks: Level[];
}

const OrderBook: React.FC<Props> = ({ bids, asks }) => {
    // Sort asks descending for display (highest on top)? 
    // Standard is: Asks on top (sorted descending price), Bids on bottom (sorted descending price).
    // Wait, Asks: Lowest price is best. Bids: Highest price is best.
    // Usually displayed:
    // Asks (High -> Low)
    // --- Spread ---
    // Bids (High -> Low)

    const sortedAsks = [...asks].sort((a, b) => b.price - a.price);
    const sortedBids = [...bids].sort((a, b) => b.price - a.price);

    const maxVol = Math.max(
        ...bids.map(b => b.qty),
        ...asks.map(a => a.qty),
        1
    );

    return (
        <div className="order-book">
            <h3>Order Book</h3>
            <div className="book-header">
                <span>Price</span>
                <span>Size</span>
                <span>Total</span>
            </div>
            <div className="asks">
                {sortedAsks.map((level, i) => (
                    <div key={i} className="level ask">
                        <div className="bar" style={{ width: `${(level.qty / maxVol) * 100}%` }}></div>
                        <span className="price">{level.price.toFixed(2)}</span>
                        <span className="qty">{level.qty}</span>
                        <span className="total">{(level.price * level.qty).toFixed(2)}</span>
                    </div>
                ))}
            </div>
            <div className="spread">
                Spread: {asks.length > 0 && bids.length > 0 ? (asks[asks.length - 1].price - bids[0].price).toFixed(2) : '-'}
            </div>
            <div className="bids">
                {sortedBids.map((level, i) => (
                    <div key={i} className="level bid">
                        <div className="bar" style={{ width: `${(level.qty / maxVol) * 100}%` }}></div>
                        <span className="price">{level.price.toFixed(2)}</span>
                        <span className="qty">{level.qty}</span>
                        <span className="total">{(level.price * level.qty).toFixed(2)}</span>
                    </div>
                ))}
            </div>
        </div>
    );
};

export default OrderBook;
