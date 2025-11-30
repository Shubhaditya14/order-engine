#pragma once

#include "OrderBook.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <variant>
#include <functional>
#include <atomic>

namespace ome {

struct Command {
    enum Type { Add, Cancel, Stop };
    Type type;
    std::optional<Order> order;
    std::optional<OrderId> orderId;
};

class MatchingEngine {
public:
    using TradeCallback = std::function<void(const std::vector<Trade>&)>;
    using BookUpdateCallback = std::function<void()>;

    MatchingEngine();
    ~MatchingEngine();

    void start();
    void stop();

    void addOrder(Order order);
    void cancelOrder(OrderId orderId);

    void setTradeCallback(TradeCallback cb);
    void setBookUpdateCallback(BookUpdateCallback cb);

    // Direct access for snapshot (thread-unsafe if engine running, use with care or add lock)
    // For this simple implementation, we'll assume single consumer of this data or add a lock.
    OrderBook& getOrderBook(); 

private:
    void run();

    OrderBook orderBook;
    std::queue<Command> commandQueue;
    std::mutex queueMutex;
    std::condition_variable queueCv;
    std::atomic<bool> running;
    std::thread engineThread;

    TradeCallback onTrade;
    BookUpdateCallback onBookUpdate;
};

} // namespace ome
