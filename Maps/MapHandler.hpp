#ifndef MAP_HANDLER
#define MAP_HANDLER

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../Utils/JSON.hpp"
#include "EventHandler.hpp"
#include "TileEvent.hpp"


class MapHandler{
    private:        
        int width = -1;
        int length = -1;
        int height = -1;
        
        //height, x, y
        char*** map;
        
        EventHandler EH;
        
        //alias conversions
        std::unordered_map<std::string, char> TILE_TAG_TO_ID;
        std::unordered_map<char, std::string> TILE_ID_TO_TAG;
        
        //parse labels
        bool parseLine(std::string line, int n);
        
        //parse map tile
        void parseTile(std::string line, int z, int x, int y, int n);
    public:
        MapHandler(std::string url);
        
        ~MapHandler();
        
        // Copy constructor (deleted to prevent accidental copies)
        MapHandler(const MapHandler& other) = delete;

        // Move constructor
        MapHandler(MapHandler&& other) noexcept;

        // Move assignment operator
        MapHandler& operator=(MapHandler&& other) noexcept;
        
        char** getLayer(int z);
        
        //get all of the tile ID and Tag pairs
        std::vector<std::pair<char, std::string>> getPairs();
        
        void triggerEvent(std::string event, int x, int y, int z);

        // Change a specific tile to a char
        void changeTile(int currZ, int currX, int currY, char toChange);
        
        void attachEventLambda(std::string tag, std::function<void(int, int, int)> func);
        
        void removeEventLambda(std::string tag);
        
        //convert map to json for sending
        json::array getJSONArr() const;
        
        //convert mappings to json for sending
        json::array getPairArr() const;
        
        char getTile(int x, int y, int z) const;
        
        bool hasTileID(char tile) const;
        
        bool hasTileTag(std::string tile) const;
        
        std::string getTagFromID(char tile);
        
        char getIDFromTag(std::string tile);
        
        int getHeight() const;
        
        int getWidth() const;
        
        int getLength() const;
};

#endif