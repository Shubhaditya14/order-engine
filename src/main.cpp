#include "engine/MatchingEngine.hpp"
#include "server/Server.hpp"
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    try {
        ome::MatchingEngine engine;
        ome::Server server(8080, engine);

        // Wire up callbacks
        engine.setTradeCallback([&server](const std::vector<ome::Trade>& trades) {
            json j;
            j["type"] = "trade";
            std::vector<json> tradeList;
            for (const auto& t : trades) {
                tradeList.push_back({
                    {"price", t.price},
                    {"qty", t.quantity},
                    {"maker", t.makerOrderId},
                    {"taker", t.takerOrderId}
                });
            }
            j["trades"] = tradeList;
            server.broadcast(j.dump());
        });

        engine.setBookUpdateCallback([&server, &engine]() {
            auto& book = engine.getOrderBook();
            json j;
            j["type"] = "book";
            
            std::vector<json> bids, asks;
            for (const auto& level : book.getBids()) {
                bids.push_back({{"price", level.price}, {"qty", level.quantity}});
            }
            for (const auto& level : book.getAsks()) {
                asks.push_back({{"price", level.price}, {"qty", level.quantity}});
            }
            j["bids"] = bids;
            j["asks"] = asks;
            
            server.broadcast(j.dump());
        });

        std::cout << "Starting Matching Engine..." << std::endl;
        engine.start();

        std::cout << "Starting WebSocket Server on port 8080..." << std::endl;
        server.run(); // Blocks

        engine.stop();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
