#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <chrono>

namespace ome {

using Price = uint64_t;
using Quantity = uint64_t;
using OrderId = uint64_t;

enum class Side {
    Buy,
    Sell
};

enum class OrderType {
    Limit,
    Market
};

struct Order {
    OrderId id;
    Side side;
    Price price;
    Quantity initialQuantity;
    Quantity remainingQuantity;
    std::chrono::system_clock::time_point timestamp;

    Order(OrderId id, Side side, Price price, Quantity qty)
        : id(id), side(side), price(price), initialQuantity(qty), remainingQuantity(qty),
          timestamp(std::chrono::system_clock::now()) {}
    
    bool isFilled() const { return remainingQuantity == 0; }
};

struct Trade {
    Price price;
    Quantity quantity;
    OrderId makerOrderId;
    OrderId takerOrderId;
    std::chrono::system_clock::time_point timestamp;
};

// Level info for GUI
struct LevelInfo {
    Price price;
    Quantity quantity;
};

} // namespace ome
