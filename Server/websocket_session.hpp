//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#ifndef BOOST_BEAST_EXAMPLE_WEBSOCKET_CHAT_MULTI_WEBSOCKET_SESSION_HPP
#define BOOST_BEAST_EXAMPLE_WEBSOCKET_CHAT_MULTI_WEBSOCKET_SESSION_HPP

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "beast.hpp"
#include "net.hpp"
#include "shared_state.hpp"
#include "../Utils/JSON.hpp"
#include "../Maps/MapHandler.hpp"

namespace json = boost::json;

// Forward declaration
class shared_state;

struct Player {
    int x = 750;
    int y = 500;
    double movDir = 0;
    double facing = 0;
    bool isMov = false;

    std::vector<std::string> inventory;
    std::string heldItem = "";
};

class ClientInstance {
   private:
    Player player;
    std::function<void(std::string)> sendPtr;
    std::vector<int> puzzle_sequence = {1, 1, 2, 5, 5, 3, 4}; 
    int puzzle_progress = 0;
    void sendPuzzleButtonPressFeedback(int button_id);
    void handlePuzzleButtonPress(int button_id);
    
    MapHandler MH = MapHandler("Maps/MapAssets/CLOSET.txt");

    void moveTo(json::object data);
    void sendMap(std::string directory);
    void startMap(int canvasW, int canvasH);
    void sendAlert(const std::string message);
    
    void interact(json::object data);
    void pickupItem(std::string item);
    void swapItem();
    void updateInventory();
    
    void tick(json::object data);

    int TILE_SIZE = 50;
    static constexpr int COLLIDER_RADIUS = 10;
    int mapOffsetX = 0;
    int mapOffsetY = 0;

    bool isWall(char id) const;
    bool isWalkable(float worldX, float worldY, int z = 0) const;


    

   public:
    ClientInstance(std::function<void(std::string)> ptr): sendPtr(ptr) {};

    void onMsg(std::string input);
};

/** Represents an active WebSocket connection to the server
 */
class websocket_session : public boost::enable_shared_from_this<websocket_session> {
    beast::flat_buffer buffer_;
    websocket::stream<beast::tcp_stream> ws_;
    boost::shared_ptr<shared_state> state_;
    std::vector<boost::shared_ptr<std::string const>> queue_;

    void fail(beast::error_code ec, char const* what);
    void on_accept(beast::error_code ec);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    
    ClientInstance* inst;

   public:
    websocket_session(tcp::socket&& socket, boost::shared_ptr<shared_state> const& state);

    ~websocket_session();

    template <class Body, class Allocator>
    void run(http::request<Body, http::basic_fields<Allocator>> req);

    // Send a message
    void send(std::string str);
    void send(boost::shared_ptr<std::string const> const& ss);

   private:
    void on_send(boost::shared_ptr<std::string const> const& ss);
};

template <class Body, class Allocator>
void websocket_session::run(http::request<Body, http::basic_fields<Allocator>> req) {
    // Set suggested timeout settings for the websocket
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(websocket::stream_base::decorator([](websocket::response_type& res) { res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-chat-multi"); }));

    // Accept the websocket handshake
    ws_.async_accept(req, beast::bind_front_handler(&websocket_session::on_accept, shared_from_this()));
}

#endif
