#include "EventHandler.hpp"

std::function<void(int, int, int)> EventHandler::EMPTY_LAMBDA = [](int, int, int){};

void EventHandler::addEvent(std::string tag, std::string type){
    if(hasEventTag(tag)){
        return;
    }
    if(TileEvent::isEventType(type)){
        EVENT_TAG_TO_TYPE[tag] = type;
    }
}

void EventHandler::addTriggerPos(std::string tag, int x, int y, int z){
    ClusterPos c{tag, x, y, z};
    POS_TO_LAMBDA[c] = EventHandler::EMPTY_LAMBDA;
    TAG_TO_POS[tag].push_back(c);
}

void EventHandler::addTriggerTile(std::string tag, char tile){
    ClusterTile c{tag, tile};
    TILE_TO_LAMBDA[c] = EventHandler::EMPTY_LAMBDA;
    TAG_TO_TILE[tag].push_back(c);
    
}

void EventHandler::dispatchEvent(std::string event, int x, int y, int z, char tile){
    if(!hasEventType(event))return;
    for(const auto& [tag, type]: EVENT_TAG_TO_TYPE){
        if(type != event)continue;
        
        for(const ClusterPos& c: TAG_TO_POS[tag]){
            if(c.x == x && c.y == y && c.z == z){
                POS_TO_LAMBDA[c](x, y, z);
                break;
            }
        }
        for(const ClusterTile& c: TAG_TO_TILE[tag]){
            if(c.tile == tile || c.tile == '*'){
                TILE_TO_LAMBDA[c](x, y, z);
                break;
            }
        }
    }
}

void EventHandler::attachEventLambda(std::string tag, std::function<void(int, int, int)> func){
    for(ClusterTile c: TAG_TO_TILE[tag]){
        TILE_TO_LAMBDA[c] = func;
    }
    for(ClusterPos c: TAG_TO_POS[tag]){
        POS_TO_LAMBDA[c] = func;
    }
}

void EventHandler::removeEventLambda(std::string tag){
    for(ClusterTile c: TAG_TO_TILE[tag]){
        TILE_TO_LAMBDA.erase(c);
    }
    for(ClusterPos c: TAG_TO_POS[tag]){
        POS_TO_LAMBDA.erase(c);
    }
}

bool EventHandler::hasEventTag(std::string tag){
    return EVENT_TAG_TO_TYPE.count(tag) > 0;
}

bool EventHandler::hasEventType(std::string type){
    return TileEvent::isEventType(type);
}