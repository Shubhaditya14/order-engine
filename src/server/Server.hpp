#pragma once

#include "engine/MatchingEngine.hpp"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include <mutex>
#include <memory>
#include <string>

namespace ome {

using WSServer = websocketpp::server<websocketpp::config::asio>;
using ConnectionHdl = websocketpp::connection_hdl;

class Server {
public:
    Server(uint16_t port, MatchingEngine& engine);
    void run();
    void stop();
    void broadcast(const std::string& message);

private:
    void onOpen(ConnectionHdl hdl);
    void onClose(ConnectionHdl hdl);
    void onMessage(ConnectionHdl hdl, WSServer::message_ptr msg);

    WSServer server;
    uint16_t port;
    MatchingEngine& engine;

    std::set<ConnectionHdl, std::owner_less<ConnectionHdl>> connections;
    std::mutex connectionsMutex;
};

} // namespace ome
