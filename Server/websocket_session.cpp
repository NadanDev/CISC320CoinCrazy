//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#include "websocket_session.hpp"


// Forward declaration
class shared_state;
class websocket_session;

void ClientInstance::moveTo(json::object data) {
    if(data.at("dir").is_double()){
        player.movDir = data.at("dir").as_double();
    }else{
        player.movDir = 0;
    }
    if(data.at("mov").is_bool()){
        player.isMov = data.at("mov").as_bool();
    }

    if(!data.at("isUp").as_bool()){
        if (data.at("dir").is_double()){
            player.facing = data.at("dir").as_double();
        }
        else{
            player.facing = 0;
        }
    }
}

void ClientInstance::sendMap(std::string directory) {
    MH = MapHandler(directory);
    json::object out;
    out["tag"] = "map";
    out["map"] = MH.getJSONArr();
    this->sendPtr(json::serialize(out));
    
    //setup event for tile change
    MH.attachEventLambda("CHANGE_TILE", [this](int x, int y, int z){
        json::object out;
        out["tag"] = "update_tile";
        json::object tile;
        tile["x"] = x;
        tile["y"] = y;
        tile["z"] = z;
        tile["type"] = MH.getTile(x, y, z);
        
        out["tile"] = tile;
        this->sendPtr(json::serialize(out));
    });
}

// This will run at the beginning, when the html loads
void ClientInstance::startMap(int canvasW, int canvasH) {
    sendMap("Maps/MapAssets/CLOSET.txt");

    // compute TILE_SIZE and map offsets the same way tick/interact do
    TILE_SIZE = std::floor(canvasW / 30);
    mapOffsetX = static_cast<int>(std::round((canvasW / 2) - (MH.getWidth() * TILE_SIZE / 2)));
    mapOffsetY = static_cast<int>(std::round((canvasH / 2) - (MH.getLength() * TILE_SIZE / 2)));

    // compute tile coords from player world coords
    int tx = static_cast<int>(std::floor((player.x - mapOffsetX) / TILE_SIZE));
    int ty = static_cast<int>(std::floor((player.y - mapOffsetY) / TILE_SIZE));
    int tz = 0;

    // guard: ensure indices are in-range before triggering
    if (tx >= 0 && ty >= 0 && tx < MH.getWidth() && ty < MH.getLength()) {
        MH.triggerEvent("start", tx, ty, tz);
    }
}

void ClientInstance::sendAlert(const std::string message) {
    // This can be used as the "dialog boxes" and to send the puzzle clue to the user
    json::object out;
    out["tag"] = "alert";
    out["msg"] = message;
    this->sendPtr(json::serialize(out));
}

void ClientInstance::interact(json::object data){
    int canvasW = data.at("width").as_int64();
    int canvasH = data.at("height").as_int64();

    mapOffsetX = static_cast<int>(std::round((canvasW / 2) - (MH.getWidth() * TILE_SIZE / 2)));
    mapOffsetY = static_cast<int>(std::round((canvasH / 2) - (MH.getLength() * TILE_SIZE / 2)));

    TILE_SIZE = std::floor(canvasW / 30);

    // When user interacts, check for the current/closest tile they are on
    int tx = static_cast<int>(std::floor((player.x - mapOffsetX) / TILE_SIZE));
    int ty = static_cast<int>(std::floor((player.y - mapOffsetY) / TILE_SIZE));
    int tz = 1; // z = 1 is the layer with all the objects
    char t = MH.getTile(tx, ty, tz);

    static const std::unordered_map<char, int> button_map = {
        {'P', 1}, {'Q', 2}, {'R', 3}, {'S', 4}, {'T', 5}
    };

    auto it = button_map.find(t);
    if (it != button_map.end()) {
        int button_id = it->second;
        handlePuzzleButtonPress(button_id);
        return;
    }
    
    // Check for tile mapping; if A and player has joystick, switch to minesweeper map. 
    // If C, switch to no crowbar map (to give the illusion that we picked it up).
    // If B and player has crowbar equipped, give puzzle clue.
    if (t == 'A'){
        if (player.heldItem == "joystick") {
            sendAlert("The joystick fits! The arcade machine works now!");
            sendMap("Maps/MapAssets/END_SCENE.txt");
        }
        else if (player.heldItem == "crowbar") {
            sendAlert("I don't think breaking the arcade machine is the right answer...");
        }
        else {
            sendAlert("Hmm, the arcade machine is broken. It looks like I need some sort of controller...");
        }
        // std::cout << "Map sent" << std::endl;
        // MH.triggerEvent("Change_Map", tx, ty, tz);
    }
    if (t == 'C'){
        pickupItem("crowbar");
        MH.changeTile(tz, tx, ty, 'a'); // THIS IS WHAT I ADDED FOR CHANGETILE
        // sendMap("Maps/MapAssets/CLOSET_WITHOUT_CROWBAR.txt"); // I UNCOMMENTED THIS BACK SO THAT IT WORKS AGAIN
        sendAlert("I got a crowbar! I wonder if there's anything I can break open with it..."
                    "(Press Q to equip/cycle through inventory)");
        // std::cout << "No crowbar map sent" << std::endl;
        // MH.triggerEvent("Pickup_Crowbar", tx, ty, tz);
    }
    if (t == 'J'){
        pickupItem("joystick");
        sendMap("Maps/MapAssets/CLOSET_WITHOUT_JOYSTICK.txt");
        sendAlert("I got a joystick! It seems like it would fit in that broken arcade machine over there...");
    }

    int offX = static_cast<int>(std::round(std::sin(player.facing)));
    int offY = static_cast<int>(std::round(std::cos(player.facing)));
    int ftx = tx + offX;
    int fty = ty + offY;

    if (ftx >= 0 && fty >= 0 && ftx < MH.getWidth() && fty < MH.getLength()) {
        char frontTile = MH.getTile(ftx, fty, tz);
        if (frontTile == 'X') {
            if (player.heldItem == "crowbar") {
                
                bool break_box = true;
                
                json::object feedback;
                feedback["tag"] = "break";
                feedback["break_box"] = break_box;
                this->sendPtr(json::serialize(feedback));

                sendMap("Maps/MapAssets/PAINT_SPLATTERS.txt");
                sendAlert("Twice the shade of grapes you'll seek,\n"
                    "Followed by burning flame, unique.\n"
                    "Then dive in the sea two times more,\n"
                    "Meadow's grass awaits just once explore,\n"
                    "And finally, step out into the sun once to complete your run.");
            } else {
                sendAlert("I think I need something to break this painted box...");
            }
        }
        else if (frontTile == 'B') {
            sendAlert("These just seem like normal boxes to me...");
        }
    }

    // Attach lambda functions - commented out because it seems kinda redundant for this tbh (- Michelle)
    // MH.attachEventLambda("Change_Map", [this](int x, int y, int z) {
    //     sendMap("Maps/MapAssets/START_MAP.txt");
    // });

    // MH.attachEventLambda("Pickup_Crowbar", [=, this](int x, int y, int z) {
    //     pickupItem("crowbar");
    //     sendAlert("I got a crowbar! I wonder if there's anything I can break open with it...");
    //     MH.changeTile(tz, tx, ty, 'a');
    // });
}

// Simple function for any item the player will pick up
void ClientInstance::pickupItem(std::string item) {
    player.inventory.push_back(item);
}

void ClientInstance::swapItem() {
    if (player.inventory.size() == 0) {
        sendAlert("You have no items to swap!");
        return;
    }

    // Swap held item with the next one in inventory
    auto index = std::find(player.inventory.begin(), player.inventory.end(), player.heldItem);
    if (player.heldItem == "") {
        player.heldItem = player.inventory.front();
    }
    else if (index == player.inventory.end() || std::next(index) == player.inventory.end()) {
        player.heldItem = "";
    } else {
        player.heldItem = *(std::next(index));
    }
    updateInventory();
}

void ClientInstance::updateInventory() {
    json::object out;
    out["tag"] = "inventory";
    out["item"] = player.heldItem;
    this->sendPtr(json::serialize(out));
}

void ClientInstance::tick(json::object data) {
    const float SPEED = 10;

    int canvasW = data.at("width").as_int64();
    int canvasH = data.at("height").as_int64();
    TILE_SIZE = std::floor(canvasW / 30);

    mapOffsetX = static_cast<int>(std::round((canvasW / 2) - (MH.getWidth() * TILE_SIZE / 2)));
    mapOffsetY = static_cast<int>(std::round((canvasH / 2) - (MH.getLength() * TILE_SIZE / 2)));

    if(player.isMov){
        float dx = std::sin(player.movDir) * SPEED;
        float dy = std::cos(player.movDir) * SPEED;

        // Try X first
        float nx = player.x + dx;
        bool canMoveX =
            isWalkable(nx - COLLIDER_RADIUS, player.y - COLLIDER_RADIUS, MH.getHeight() - 1) &&
            isWalkable(nx + COLLIDER_RADIUS, player.y - COLLIDER_RADIUS, MH.getHeight() - 1) &&
            isWalkable(nx - COLLIDER_RADIUS, player.y + COLLIDER_RADIUS, MH.getHeight() - 1) &&
            isWalkable(nx + COLLIDER_RADIUS, player.y + COLLIDER_RADIUS, MH.getHeight() - 1);

        if (canMoveX) {
            player.x = nx;
        }

        // Then Y
        float ny = player.y + dy;
        bool canMoveY =
            isWalkable(player.x - COLLIDER_RADIUS, ny - COLLIDER_RADIUS, MH.getHeight() - 1) &&
            isWalkable(player.x + COLLIDER_RADIUS, ny - COLLIDER_RADIUS, MH.getHeight() - 1) &&
            isWalkable(player.x - COLLIDER_RADIUS, ny + COLLIDER_RADIUS, MH.getHeight() - 1) &&
            isWalkable(player.x + COLLIDER_RADIUS, ny + COLLIDER_RADIUS, MH.getHeight() - 1);

        if (canMoveY) {
            player.y = ny;
        }
    }

    json::object out;
    out["tag"] = "mov";
    json::object pos;
    pos["x"] = player.x;
    pos["y"] = player.y;
    out["data"] = pos;

    this->sendPtr(json::serialize(out));
}

bool ClientInstance::isWall(char id) const {
    // Currently player can walk on top of crowbar; if we don't want that simply add case 'C': at the end
    // 'B' is also commented out, because currently the logic is if player is on the tile then player can break boxes;
    // With collision the player can never actually reach them. Will figure out a way for both to work (- Michelle)
    switch (id) {
        case '=':
        case '^':
        case '[':
        case ']':
        case 'B': 
        case 'X':
            return true;
        default:
        return false;
    }
}

bool ClientInstance::isWalkable(float worldX, float worldY, int z) const {
    // Convert world to tile coords
    float localX = worldX - mapOffsetX;
    float localY = worldY - mapOffsetY;

    if (localX < 0 || localY < 0) return false;

    int tx = static_cast<int>(std::floor(localX / TILE_SIZE));
    int ty = static_cast<int>(std::floor(localY / TILE_SIZE));

    if (tx < 0 || ty < 0 || tx >= MH.getWidth() || ty >= MH.getLength()) return false;

    char t = MH.getTile(tx, ty, z);
    return !isWall(t);
}

void ClientInstance::handlePuzzleButtonPress(int button_id) {
    
    json::object feedback;
    feedback["tag"] = "button_feedback";
    feedback["button_id"] = button_id;
    this->sendPtr(json::serialize(feedback));

    if (button_id == puzzle_sequence[puzzle_progress]) {
        puzzle_progress++;
        if (puzzle_progress == puzzle_sequence.size()) {
            sendAlert("Good job! You've solved the puzzle. Try using this joystick somewhere!");
            sendMap("Maps/MapAssets/CLOSET_WITH_JOYSTICK.txt");
            puzzle_progress = 0;
        }
    } else {
        sendAlert("Incorrect button! Try again from the start.");
        puzzle_progress = 0;
    }
}

void ClientInstance::onMsg(std::string input) {
    boost::system::error_code ec;
    json::value data = json::parse(input, ec);
    if (ec) {
        std::cerr << "Error parsing JSON: " << ec.message() << std::endl;
        return;
    }
    if(!data.at("tag").is_string()){
        std::cerr << "NO TAG FOUND: " << std::endl;
        return;
    }

    if (data.at("tag").as_string() == "mov") {
        moveTo(data.at("data").as_object());
    }
    
    if (data.at("tag").as_string() == "tick") {
        tick(data.at("data").as_object());
    }
    
    if(data.at("tag").as_string() == "interact"){
        interact(data.at("data").as_object());
        
    }

    if(data.at("tag").as_string() == "press"){
        interact(data.at("data").as_object());
        
    }

    if (data.at("tag").as_string() == "swapItem"){
        // Press "Q" while something in inventory to call this
        swapItem();
    }

    if (data.at("tag").as_string() == "start"){
        player.x = data.at("data").as_object().at("width").as_int64() / 2;
        player.y = data.at("data").as_object().at("height") .as_int64() / 2;
        startMap(static_cast<int>(data.at("data").as_object().at("width").as_int64()),
                 static_cast<int>(data.at("data").as_object().at("height").as_int64()));
    }

    if (data.at("tag").as_string() == "button_press") {
        int btn_id = static_cast<int>(data.at("data").as_object().at("buttonId").as_int64());
        handlePuzzleButtonPress(btn_id);
    }

}

websocket_session::websocket_session(tcp::socket&& socket, boost::shared_ptr<shared_state> const& state) : ws_(std::move(socket)), state_(state) {
    //void (websocket_session::*send)(std::string) = &websocket_session.send;
    std::function<void(std::string)> send = [this](std::string msg){
        this->send(msg);
    };
    inst = new ClientInstance(send);
}

websocket_session::~websocket_session() {
    // Remove this session from the list of active sessions
    state_->leave(this);
    delete this->inst;
}

void websocket_session::fail(beast::error_code ec, char const* what) {
    // Don't report these
    if (ec == net::error::operation_aborted || ec == websocket::error::closed) return;

    std::cerr << what << ": " << ec.message() << "\n";
}

void websocket_session::on_accept(beast::error_code ec) {
    // Handle the error, if any
    if (ec) return fail(ec, "accept");

    // Add this session to the list of active sessions
    state_->join(this);

    // Read a message
    ws_.async_read(buffer_, beast::bind_front_handler(&websocket_session::on_read, shared_from_this()));
}

void websocket_session::on_read(beast::error_code ec, std::size_t) {
    // Handle the error, if any
    if (ec) return fail(ec, "read");

    // Send to all connections
    //state_->send_all(beast::buffers_to_string(buffer_.data()));
    inst->onMsg(beast::buffers_to_string(buffer_.data()));

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Read another message
    ws_.async_read(buffer_, beast::bind_front_handler(&websocket_session::on_read, shared_from_this()));
}


void websocket_session::send(std::string str) {
    // Post our work to the strand, this ensures
    // that the members of `this` will not be
    // accessed concurrently.
    
    auto const ss = boost::make_shared<std::string const>(std::move(str));

    this->send(ss);
    //net::post(ws_.get_executor(), beast::bind_front_handler(&websocket_session::on_send, shared_from_this(), ss));
}

void websocket_session::send(boost::shared_ptr<std::string const> const& ss) {
    // Post our work to the strand, this ensures
    // that the members of `this` will not be
    // accessed concurrently.

    net::post(ws_.get_executor(), beast::bind_front_handler(&websocket_session::on_send, shared_from_this(), ss));
}

void websocket_session::on_send(boost::shared_ptr<std::string const> const& ss) {
    // Always add to queue
    queue_.push_back(ss);

    // Are we already writing?
    if (queue_.size() > 1) return;

    // We are not currently writing, so send this immediately
    ws_.async_write(net::buffer(*queue_.front()), beast::bind_front_handler(&websocket_session::on_write, shared_from_this()));
}

void websocket_session::on_write(beast::error_code ec, std::size_t) {
    // Handle the error, if any
    if (ec) return fail(ec, "write");

    // Remove the string from the queue
    queue_.erase(queue_.begin());

    // Send the next message if any
    if (!queue_.empty()) ws_.async_write(net::buffer(*queue_.front()), beast::bind_front_handler(&websocket_session::on_write, shared_from_this()));
}