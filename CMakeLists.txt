cmake_minimum_required(VERSION 3.15)
project(OrderMatchingEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(Threads REQUIRED)

include(FetchContent)

# JSON
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Asio (Standalone)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)
FetchContent_MakeAvailable(asio)

# WebSocket++
FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG 0.8.2
)
FetchContent_MakeAvailable(websocketpp)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/common/*.cpp"
    "src/engine/*.cpp"
    "src/server/*.cpp"
    "src/main.cpp"
)

# Executable
add_executable(ome ${SOURCES})

target_include_directories(ome PRIVATE 
    src
    ${asio_SOURCE_DIR}/asio/include
    ${websocketpp_SOURCE_DIR}
)

target_compile_definitions(ome PRIVATE ASIO_STANDALONE)
target_link_libraries(ome PRIVATE nlohmann_json::nlohmann_json Threads::Threads)
