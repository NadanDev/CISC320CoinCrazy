cmake_minimum_required(VERSION 3.20)
project(MyServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB SERVER_SOURCES
    server/*.cpp
    Maps/*.cpp
    Utils/*.cpp
    main.cpp
)

# Include Boost headers
include_directories("C:/local/boost_1_89_0")

# Add executable
add_executable(Coin-Crazy-Escape ${SERVER_SOURCES})

# Manually link Boost libraries
target_link_libraries(Coin-Crazy-Escape
    "C:/local/boost_1_89_0/stage/lib/libboost_filesystem-mgw15-mt-x64-1_89.a"
    "C:/local/boost_1_89_0/stage/lib/libboost_atomic-mgw15-mt-x64-1_89.a"
    ws2_32
    mswsock
)