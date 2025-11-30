import React, { useState } from 'react';
import type { Side } from '../App';
import './OrderEntry.css';

interface Props {
    onSendOrder: (side: Side, price: number, qty: number) => void;
}

const OrderEntry: React.FC<Props> = ({ onSendOrder }) => {
    const [side, setSide] = useState<Side>('buy');
    const [price, setPrice] = useState<string>('100');
    const [qty, setQty] = useState<string>('10');

    const handleSubmit = (e: React.FormEvent) => {
        e.preventDefault();
        onSendOrder(side, parseFloat(price), parseInt(qty));
    };

    return (
        <div className="order-entry">
            <h3>Place Order</h3>
            <div className="tabs">
                <button
                    className={side === 'buy' ? 'active buy' : ''}
                    onClick={() => setSide('buy')}
                >
                    BUY
                </button>
                <button
                    className={side === 'sell' ? 'active sell' : ''}
                    onClick={() => setSide('sell')}
                >
                    SELL
                </button>
            </div>
            <form onSubmit={handleSubmit}>
                <div className="field">
                    <label>Price</label>
                    <input
                        type="number"
                        value={price}
                        onChange={e => setPrice(e.target.value)}
                        step="0.01"
                    />
                </div>
                <div className="field">
                    <label>Quantity</label>
                    <input
                        type="number"
                        value={qty}
                        onChange={e => setQty(e.target.value)}
                    />
                </div>
                <button type="submit" className={`submit-btn ${side}`}>
                    {side.toUpperCase()}
                </button>
            </form>
        </div>
    );
};

export default OrderEntry;
