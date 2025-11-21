#include "TileEvent.hpp"

bool ClusterPos::operator==(const ClusterPos& other) const{
    return tag == other.tag &&
        x == other.x && 
        y == other.y &&
        z == other.z;
}

bool ClusterTile::operator==(const ClusterTile& other) const{
    return tag == other.tag &&
        tile == other.tile;
}

namespace std{
    std::size_t hash<ClusterPos>::operator()(const ClusterPos& key) const{
        std::size_t seed = 0;
        boost::hash_combine(seed, key.tag);
        boost::hash_combine(seed, key.x);
        boost::hash_combine(seed, key.y);
        boost::hash_combine(seed, key.z);
        return seed;
    }
    std::size_t hash<ClusterTile>::operator()(const ClusterTile& key) const{
        std::size_t seed = 0;
        boost::hash_combine(seed, key.tag);
        boost::hash_combine(seed, key.tile);
        return seed;
    }
}

std::set<std::string> TileEvent::EventType = {
    "onstep",       //triggers only on initial step
    "holdstep",     //triggers on step(continuous)
    "offstep",      //triggers on step off
    
    "tilechange",    //triggers on tile change
    
    "interact"     // triggers when player interacts
};

bool TileEvent::isEventType(std::string tag){
    return EventType.find(tag) != EventType.end();
}