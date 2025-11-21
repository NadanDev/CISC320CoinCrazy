#ifndef TILE_EVENT
#define TILE_EVENT

#include <string>
#include <set>
#include <functional>

#include <boost/container_hash/hash.hpp>

struct ClusterPos{
    std::string tag;
    int x;
    int y;
    int z;
    
    bool operator==(const ClusterPos& other) const;
};

struct ClusterTile{
    std::string tag;
    char tile;
    
    bool operator==(const ClusterTile& other) const;
};

namespace std{
    template <>
    struct hash<ClusterPos>{
        std::size_t operator()(const ClusterPos& key) const;
    };
    
    template<>
    struct hash<ClusterTile>{
        std::size_t operator()(const ClusterTile& key) const;
    };
}

class TileEvent{
    private:
        //enum conversions
        static std::set<std::string> EventType;
    public:
        static bool isEventType(std::string tag);
};

#endif