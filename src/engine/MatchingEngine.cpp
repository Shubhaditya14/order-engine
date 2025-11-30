#include "MatchingEngine.hpp"
#include <iostream>

namespace ome {

MatchingEngine::MatchingEngine() : running(false) {}

MatchingEngine::~MatchingEngine() {
    stop();
}

void MatchingEngine::start() {
    running = true;
    engineThread = std::thread(&MatchingEngine::run, this);
}

void MatchingEngine::stop() {
    if (running) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            commandQueue.push({Command::Stop, std::nullopt, std::nullopt});
        }
        queueCv.notify_one();
        if (engineThread.joinable()) {
            engineThread.join();
        }
        running = false;
    }
}

void MatchingEngine::addOrder(Order order) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        commandQueue.push({Command::Add, order, std::nullopt});
    }
    queueCv.notify_one();
}

void MatchingEngine::cancelOrder(OrderId orderId) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        commandQueue.push({Command::Cancel, std::nullopt, orderId});
    }
    queueCv.notify_one();
}

void MatchingEngine::setTradeCallback(TradeCallback cb) {
    onTrade = cb;
}

void MatchingEngine::setBookUpdateCallback(BookUpdateCallback cb) {
    onBookUpdate = cb;
}

OrderBook& MatchingEngine::getOrderBook() {
    return orderBook;
}

void MatchingEngine::run() {
    while (running) {
        Command cmd;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCv.wait(lock, [this] { return !commandQueue.empty(); });
            cmd = commandQueue.front();
            commandQueue.pop();
        }

        if (cmd.type == Command::Stop) break;

        bool bookChanged = false;
        if (cmd.type == Command::Add && cmd.order) {
            auto trades = orderBook.addOrder(*cmd.order);
            if (!trades.empty()) {
                if (onTrade) onTrade(trades);
                bookChanged = true;
            }
            // If order was added to book (not fully filled), book changed
            if (!cmd.order->isFilled()) {
                bookChanged = true;
            }
        } else if (cmd.type == Command::Cancel && cmd.orderId) {
            if (orderBook.cancelOrder(*cmd.orderId)) {
                bookChanged = true;
            }
        }

        if (bookChanged && onBookUpdate) {
            onBookUpdate();
        }
    }
}

} // namespace ome
