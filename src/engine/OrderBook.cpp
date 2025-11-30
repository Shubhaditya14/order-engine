#include "OrderBook.hpp"
#include <iostream>

namespace ome {

OrderBook::OrderBook() {}

std::vector<Trade> OrderBook::addOrder(Order order) {
    std::vector<Trade> trades;

    if (orderLookup.find(order.id) != orderLookup.end()) {
        // Duplicate order ID, reject or ignore
        return trades;
    }

    match(order, trades);

    if (!order.isFilled()) {
        if (order.side == Side::Buy) {
            addToBook(order, bids);
        } else {
            addToBook(order, asks);
        }
    }

    return trades;
}

bool OrderBook::cancelOrder(OrderId orderId) {
    auto it = orderLookup.find(orderId);
    if (it == orderLookup.end()) {
        return false;
    }

    const auto& loc = it->second;
    if (loc.side == Side::Buy) {
        auto levelIt = bids.find(loc.price);
        if (levelIt != bids.end()) {
            levelIt->second.totalVolume -= loc.iterator->remainingQuantity;
            levelIt->second.orders.erase(loc.iterator);
            if (levelIt->second.orders.empty()) {
                bids.erase(levelIt);
            }
        }
    } else {
        auto levelIt = asks.find(loc.price);
        if (levelIt != asks.end()) {
            levelIt->second.totalVolume -= loc.iterator->remainingQuantity;
            levelIt->second.orders.erase(loc.iterator);
            if (levelIt->second.orders.empty()) {
                asks.erase(levelIt);
            }
        }
    }

    orderLookup.erase(it);
    return true;
}

void OrderBook::match(Order& incoming, std::vector<Trade>& trades) {
    if (incoming.side == Side::Buy) {
        matchAgainstBook(incoming, asks, trades);
    } else {
        matchAgainstBook(incoming, bids, trades);
    }
}

template<typename BookSide>
void OrderBook::matchAgainstBook(Order& incoming, BookSide& book, std::vector<Trade>& trades) {
    auto it = book.begin();
    while (it != book.end() && !incoming.isFilled()) {
        Level& level = it->second;
        
        // Price check
        bool priceMatch = (incoming.side == Side::Buy) ? (incoming.price >= level.price) : (incoming.price <= level.price);
        if (!priceMatch) break;

        auto orderIt = level.orders.begin();
        while (orderIt != level.orders.end() && !incoming.isFilled()) {
            Order& bookOrder = *orderIt;
            
            Quantity tradeQty = std::min(incoming.remainingQuantity, bookOrder.remainingQuantity);
            
            trades.push_back({
                level.price,
                tradeQty,
                bookOrder.id,
                incoming.id,
                std::chrono::system_clock::now()
            });

            incoming.remainingQuantity -= tradeQty;
            bookOrder.remainingQuantity -= tradeQty;
            level.totalVolume -= tradeQty;

            if (bookOrder.isFilled()) {
                orderLookup.erase(bookOrder.id);
                orderIt = level.orders.erase(orderIt);
            } else {
                ++orderIt;
            }
        }

        if (level.orders.empty()) {
            it = book.erase(it);
        } else {
            ++it;
        }
    }
}

template<typename BookSide>
void OrderBook::addToBook(Order& order, BookSide& book) {
    auto& level = book[order.price];
    if (level.totalVolume == 0) {
        level.price = order.price; // Initialize if new
    }
    
    level.orders.push_back(order);
    level.totalVolume += order.remainingQuantity;
    
    // Store iterator for lookup
    auto it = level.orders.end();
    --it;
    orderLookup.insert({order.id, {order.side, order.price, it}});
}

std::vector<LevelInfo> OrderBook::getBids() const {
    std::vector<LevelInfo> levels;
    for (const auto& [price, level] : bids) {
        levels.push_back({price, level.totalVolume});
    }
    return levels;
}

std::vector<LevelInfo> OrderBook::getAsks() const {
    std::vector<LevelInfo> levels;
    for (const auto& [price, level] : asks) {
        levels.push_back({price, level.totalVolume});
    }
    return levels;
}

} // namespace ome
