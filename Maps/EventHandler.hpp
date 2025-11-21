#ifndef EVENT_HANDLER
#define EVENT_HANDLER

#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

#include "TileEvent.hpp"

class EventHandler{
    private:
        //lambda that does nothing
        static std::function<void(int, int, int)> EMPTY_LAMBDA;
        
        //tile event tags with the type of event
        std::unordered_map<std::string, std::string> EVENT_TAG_TO_TYPE;
        
        //maps a trigger, x, y, z to a lambda that called (x, y, z)
        std::unordered_map<ClusterPos, std::function<void(int, int, int)>> POS_TO_LAMBDA;
        //maps a trigger on a tile to a lambda
        std::unordered_map<ClusterTile, std::function<void(int, int, int)>> TILE_TO_LAMBDA;
        
        //what event tag is match to what triggers
        std::unordered_map<std::string, std::vector<ClusterPos>> TAG_TO_POS;
        std::unordered_map<std::string, std::vector<ClusterTile>> TAG_TO_TILE;
        
    public:
        void addEvent(std::string tag, std::string type);
        
        void addTriggerPos(std::string tag, int x, int y, int z);
        
        void addTriggerTile(std::string tag, char tile);
        
        void dispatchEvent(std::string event, int x, int y, int z, char tile);
        
        void attachEventLambda(std::string tag, std::function<void(int, int, int)> func);
        
        void removeEventLambda(std::string tag);
        
        bool hasEventTag(std::string tag);
        
        bool hasEventType(std::string type);
};

#endif