#include "Server.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

namespace ome {

static std::atomic<OrderId> globalOrderId{1};

Server::Server(uint16_t port, MatchingEngine& engine)
    : port(port), engine(engine) {
    
    server.init_asio();
    
    server.set_open_handler(bind(&Server::onOpen, this, std::placeholders::_1));
    server.set_close_handler(bind(&Server::onClose, this, std::placeholders::_1));
    server.set_message_handler(bind(&Server::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    
    // Disable logging for performance/cleanliness
    server.clear_access_channels(websocketpp::log::alevel::all);
    server.set_error_channels(websocketpp::log::elevel::all);
}

void Server::run() {
    server.listen(port);
    server.start_accept();
    server.run();
}

void Server::stop() {
    server.stop();
}

void Server::onOpen(ConnectionHdl hdl) {
    {
        std::lock_guard<std::mutex> lock(connectionsMutex);
        connections.insert(hdl);
    }

    // Send snapshot
    auto& book = engine.getOrderBook();
    json j;
    j["type"] = "snapshot";
    
    std::vector<json> bids, asks;
    for (const auto& level : book.getBids()) {
        bids.push_back({{"price", level.price}, {"qty", level.quantity}});
    }
    for (const auto& level : book.getAsks()) {
        asks.push_back({{"price", level.price}, {"qty", level.quantity}});
    }
    j["bids"] = bids;
    j["asks"] = asks;
    
    try {
        server.send(hdl, j.dump(), websocketpp::frame::opcode::text);
    } catch (const websocketpp::exception& e) {
        std::cerr << "Send error: " << e.what() << std::endl;
    }
}

void Server::onClose(ConnectionHdl hdl) {
    std::lock_guard<std::mutex> lock(connectionsMutex);
    connections.erase(hdl);
}

void Server::onMessage(ConnectionHdl hdl, WSServer::message_ptr msg) {
    try {
        auto j = json::parse(msg->get_payload());
        std::string type = j["type"];

        if (type == "add") {
            Side side = (j["side"] == "buy") ? Side::Buy : Side::Sell;
            Price price = j["price"];
            Quantity qty = j["qty"];
            OrderId id = globalOrderId++;
            
            Order order(id, side, price, qty);
            engine.addOrder(order);
        } else if (type == "cancel") {
            OrderId id = j["orderId"];
            engine.cancelOrder(id);
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON error: " << e.what() << std::endl;
    }
}

void Server::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(connectionsMutex);
    for (auto it = connections.begin(); it != connections.end(); ) {
        try {
            server.send(*it, message, websocketpp::frame::opcode::text);
            ++it;
        } catch (const websocketpp::exception& e) {
            std::cerr << "Broadcast error: " << e.what() << std::endl;
            it = connections.erase(it);
        }
    }
}

} // namespace ome
