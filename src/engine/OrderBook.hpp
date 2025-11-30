#pragma once

#include "common/types.hpp"
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <optional>
#include <functional>

namespace ome {

struct OrderEntry {
    Order order;
    std::list<Order>::iterator location; // Iterator to the order in the Level's list
};

struct Level {
    Price price;
    Quantity totalVolume;
    std::list<Order> orders;

    Level() : price(0), totalVolume(0) {}
    Level(Price p) : price(p), totalVolume(0) {}
};

class OrderBook {
public:
    OrderBook();

    // Returns trades generated
    std::vector<Trade> addOrder(Order order);
    bool cancelOrder(OrderId orderId);
    
    // Getters for GUI
    std::vector<LevelInfo> getBids() const;
    std::vector<LevelInfo> getAsks() const;

private:
    // Bids: Highest price first
    std::map<Price, Level, std::greater<Price>> bids;
    // Asks: Lowest price first
    std::map<Price, Level, std::less<Price>> asks;

    // Fast lookup for cancellation
    struct OrderLocation {
        Side side;
        Price price;
        std::list<Order>::iterator iterator;
    };
    std::unordered_map<OrderId, OrderLocation> orderLookup;

    // Helpers
    void match(Order& incoming, std::vector<Trade>& trades);
    
    template<typename BookSide>
    void matchAgainstBook(Order& incoming, BookSide& book, std::vector<Trade>& trades);
    
    template<typename BookSide>
    void addToBook(Order& order, BookSide& book);
};

} // namespace ome
