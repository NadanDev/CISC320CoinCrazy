#include "MapHandler.hpp"


MapHandler::MapHandler(std::string url){
    std::ifstream reader(url);
    
    if (!reader.is_open())std::cerr << "Failed to open the file '" << url << "'" << std::endl;
    
    const std::string COMMENT_DELIM = "//";
    
    int n = 0; //line counter
    
    //read params
    std::string line;
    while (std::getline(reader, line)) {
        n++;
        line = line.substr(0, line.find(COMMENT_DELIM));
        boost::trim(line);
        
        if(!line.empty() && line[0] == '@' && parseLine(line, n))break;
    }
    
    if(width <= 0 || length <= 0 || height <= 0)throw std::invalid_argument("Invalid map size at line " + std::to_string(n));
    
    //build array
    int z = 0;
    int x = 0;
    int y = 0;
    
    map = new char**[height];
    
    for(;z < height; z++){
        map[z] = new char*[width];
        for(x = 0;x < width; x++){
            map[z][x] = new char[length];
        }
    }
    
    z = 0;
    x = 0;
    y = 0;
    
    //read map
    while (std::getline(reader, line)) {
        n++;
        // Find comment delimiter position
        auto comment_pos = line.find(COMMENT_DELIM);
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        boost::trim(line);
        if (line.empty()) continue;
        
        std::istringstream lineStream(line);
        std::string tile;
        x = 0;
        while (lineStream >> tile) {
            parseTile(tile, z, x, y, n);
            x++;
            if(x > width)throw std::invalid_argument("Map width mismatch(> width) at line " + std::to_string(n));
        }
        n++;
        if(x < width)throw std::invalid_argument("Map width mismatch(< width) at line " + std::to_string(n));
        y++;
        if(y >= length){
            y = 0;
            z++;
            if(z > height)throw std::invalid_argument("Map height/length mismatch(> height/width) at line " + std::to_string(n));
        }
    }
    if(z == 0 && x == 0 && y == 0)throw std::invalid_argument("@initmap never called/map not provided");
    if(z < height || y != 0)throw std::invalid_argument("Map height/length mismatch(< height/width) at line " + std::to_string(n));
}
    
// Destructor
MapHandler::~MapHandler(){
    if(map == nullptr)return;
    for(int z = 0; z < height; z++){
        if(map[z] == nullptr)continue;
        for(int x = 0; x < width; x++){
            if(map[z][x] == nullptr)continue;
            delete[] map[z][x];
        }
        delete[] map[z];
    }
    delete[] map;
    map = nullptr;
}

// Move constructor
MapHandler::MapHandler(MapHandler&& other) noexcept
    : map(other.map),
      width(other.width),
      length(other.length),
      height(other.height),
      TILE_ID_TO_TAG(std::move(other.TILE_ID_TO_TAG)),
      TILE_TAG_TO_ID(std::move(other.TILE_TAG_TO_ID)),
      EH(std::move(other.EH)) {
    other.map = nullptr;
    other.width = other.length = other.height = 0;
}

// Move assignment
MapHandler& MapHandler::operator=(MapHandler&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        if (map != nullptr) {
            for (int z = 0; z < height; ++z) {
                if (map[z] != nullptr) {
                    for (int x = 0; x < width; ++x) {
                        if (map[z][x] != nullptr) {
                            delete[] map[z][x];
                        }
                    }
                    delete[] map[z];
                }
            }
            delete[] map;
        }

        // Move data
        map = other.map;
        width = other.width;
        length = other.length;
        height = other.height;
        TILE_ID_TO_TAG = std::move(other.TILE_ID_TO_TAG);
        TILE_TAG_TO_ID = std::move(other.TILE_TAG_TO_ID);
        EH = std::move(other.EH);

        // Null out source
        other.map = nullptr;
        other.width = other.length = other.height = 0;
    }
    return *this;
}


char** MapHandler::getLayer(int z){
    return map[z];
}

bool MapHandler::parseLine(std::string line, int n){
    if(line.empty() || line[0] != '@')return false;
    
    std::string argStr = line.substr(1);
    std::vector<std::string> args;
    
    //seperate args
    std::istringstream lineStream(argStr);
    std::string str;
    while(lineStream >> str){
        args.push_back(str);
    }
    
    //get label
    if(args.size() == 0){
        std::cerr << "Error reading label at line " << n << std::endl;
        return false;
    }
    std::string label = args[0];
    
    //parse
    if(label == "width" && args.size() >= 2){
        //width label
        if(width != -1){
            std::cerr << "Width alreadly assigned at line " << n << std::endl;
            return false;
        }
        //read width
        try{
            width = std::stoi(args[1]);
            if(width <= 0)throw std::out_of_range("");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error reading '" << args[1] << "': Invalid argument at line " << n << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error reading '" << args[1] << "': Out of range at line " << n << std::endl;
        }
    }else if(label == "length" && args.size() >= 2){
        //length label
        if(length != -1){
            std::cerr << "Length alreadly assigned at line " << n << std::endl;
            return false;
        }
        //read length
        try{
            length = std::stoi(args[1]);
            if(length <= 0)throw std::out_of_range("");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error reading '" << args[1] << "': Invalid argument at line " << n << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error reading '" << args[1] << "': Out of range at line " << n << std::endl;
        }
    }else if(label == "height" && args.size() >= 2){
        //height label
        if(height != -1){
            std::cerr << "Height alreadly assigned at line " << n << std::endl;
            return false;
        }
        //read height
        try{
            height = std::stoi(args[1]);
            if(height <= 0)throw std::out_of_range("");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error reading '" << args[1] << "': Invalid argument at line " << n << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error reading '" << args[1] << "': Out of range at line " << n << std::endl;
        }
    }else if(label == "alias" && args.size() >= 3){
        //alias label
        std::string tileAliasStr = args[1];
        if(tileAliasStr.length() > 1){
            std::cerr << tileAliasStr << tileAliasStr.length() << std::endl;
            std::cerr << "Error reading '" << args[1] << "': Invalid argument at line " << n << std::endl;
            return false;
        }
        char tileID = tileAliasStr[0];
        std::string tileName = args[2];
        
        if(hasTileTag(tileName) || hasTileID(tileID) > 0){
            std::cerr << "Duplicate tile or alias at line " << n << std::endl;
            return false;
        }
        
        //set alias
        TILE_TAG_TO_ID[tileName] = tileID;
        TILE_ID_TO_TAG[tileID] = tileName;
    }else if(label == "event" && args.size() >= 3){
        //event label
        if(EH.hasEventTag(args[1])){
            std::cerr << "Duplicate event tag at line " << n << std::endl;
            return false;
        }
        if(!TileEvent::isEventType(args[2])){
            std::cerr << "Unknown event type at line " << n << std::endl;
            return false;
        }
        
        //assign event tag
        EH.addEvent(args[1], args[2]);
    }else if(label == "trigger-pos" && args.size() >= 5){
        if(!EH.hasEventTag(args[1])){
            std::cerr << "Unknown event tag at line " << n << std::endl;
            return false;
        }
        int x, y, z;
        
        //read x
        try{
            x = std::stoi(args[2]);
            if(x >= width || x < 0) throw std::out_of_range("");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error reading '" << args[2] << "': Invalid argument at line " << n << std::endl;
            return false;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error reading '" << args[2] << "': Out of range at line " << n << std::endl;
            return false;
        }
        
        //read y
        try{
            y = std::stoi(args[3]);
            if(y >= length || y < 0) throw std::out_of_range("");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error reading '" << args[3] << "': Invalid argument at line " << n << std::endl;
            return false;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error reading '" << args[3] << "': Out of range at line " << n << std::endl;
            return false;
        }
        
        //read z
        try{
            z = std::stoi(args[4]);
            if(z >= height || z < 0) throw std::out_of_range("");
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error reading '" << args[3] << "': Invalid argument at line " << n << std::endl;
            return false;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error reading '" << args[3] << "': Out of range at line " << n << std::endl;
            return false;
        }
        
        //sets trigger
        EH.addTriggerPos(args[1], x, y, z);
        return false;
    }else if(label == "trigger-tile" && args.size() >= 3){
        if(!EH.hasEventTag(args[1])){
            std::cerr << "Unknown event tag at line " << n << std::endl;
            return false;
        }
        
        if(args[2].length() == 1 && (hasTileID(args[2][0]) || args[2][0] == '*')){
            //sets trigger
            EH.addTriggerTile(args[1], args[2][0]);
            return false;
        }
        if(hasTileTag(args[2])){
            char tile = getIDFromTag(args[2]);
            //sets trigger
            EH.addTriggerTile(args[1], tile);
            return false;
        }
        
        std::cerr << "Unknown tile'" << args[2] << "' at line " << n << std::endl;
        
    }else if(label == "initmap"){
        //initiates map reading
        return true;
    }else{
        std::cerr << "Error reading label'" << label << "' at line " << n << std::endl;
    }
    return false;
}

void MapHandler::parseTile(std::string tile, int z, int x, int y, int n){
    if(tile.empty())return;
    
    if(tile.length() == 1 && hasTileID(tile[0])){
        map[z][x][y] = tile[0];
        return;
    }
    
    if(hasTileTag(tile)){
        map[z][x][y] = getIDFromTag(tile);
        return;
    }
    
    std::cerr << "Unknown tile'" << tile << "' at line " << n << std::endl;
}

std::vector<std::pair<char, std::string>> MapHandler::getPairs(){
    return std::vector<std::pair<char, std::string>>(TILE_ID_TO_TAG.begin(), TILE_ID_TO_TAG.end());
}

void MapHandler::triggerEvent(std::string event, int x, int y, int z){
    EH.dispatchEvent(event, x, y, z, map[z][x][y]);
}

void MapHandler::changeTile(int currZ, int currX, int currY, char toChange) {
    map[currZ][currX][currY] = toChange;
    triggerEvent("tilechange", currX, currY, currZ);
}

void MapHandler::attachEventLambda(std::string tag, std::function<void(int, int, int)> func){
    EH.attachEventLambda(tag, func);
}

void MapHandler::removeEventLambda(std::string tag){
    EH.removeEventLambda(tag);
}

json::array MapHandler::getJSONArr() const{
    json::array outerArray;
    for (size_t z = 0; z < height; ++z) {
        json::array midArray;
        for (size_t x = 0; x < width; ++x) {
            json::array innerArray;
            for (size_t y = 0; y < length; ++y) {
                std::string s(1, map[z][x][y]); 
                innerArray.emplace_back(s);
            }
            midArray.push_back(std::move(innerArray));
        }
        outerArray.push_back(std::move(midArray));
    }

    return outerArray;
}

json::array MapHandler::getPairArr() const{
    json::array pairs;
    
    for(const auto& [id, tag]: TILE_ID_TO_TAG){
        json::array pair;
        pair.emplace_back(std::string(1, id));
        pair.emplace_back(tag);
        
        pairs.push_back(pair);
    }
    
    return pairs;
}


bool MapHandler::hasTileID(char tile) const{
    return TILE_ID_TO_TAG.count(tile) > 0;
}

bool MapHandler::hasTileTag(std::string tile) const{
    return TILE_TAG_TO_ID.count(tile) > 0;
}

std::string MapHandler::getTagFromID(char tile){
    return TILE_ID_TO_TAG[tile];
}

char MapHandler::getIDFromTag(std::string tile){
    return TILE_TAG_TO_ID[tile];
}

char MapHandler::getTile(int x, int y, int z) const{
    if (map == nullptr) {
        throw std::runtime_error("MapHandler::getTile: map not initialized");
    }
    if (z < 0 || z >= height || x < 0 || x >= width || y < 0 || y >= length) {
        throw std::out_of_range("MapHandler::getTile: coordinates out of bounds");
    }
    return map[z][x][y];
}

int MapHandler::getHeight() const{
    return this->height;
}

int MapHandler::getWidth() const{
    return this->width;
}

int MapHandler::getLength() const{
    return this->length;
}

