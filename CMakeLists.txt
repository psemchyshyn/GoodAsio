
cmake_minimum_required(VERSION 3.16)
project(echo_server)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(Threads REQUIRED)
if (WIN32)
    link_libraries(ws2_32 wsock32)
endif (WIN32)

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()


add_executable(async_server my_time.h TCPServer.h TCPServer.cpp TCPConnection.cpp TCPConnection.h main.cpp)
add_executable(sync_server sync_server.cpp my_time.h)
add_executable(client client.cpp my_time.h)
add_executable(raw_sync_serv raw_sync_serv.cpp)

# Linking
target_link_libraries(sync_server ${Boost_LIBRARIES})
target_link_libraries(async_server ${Boost_LIBRARIES} Threads::Threads)
target_link_libraries(client ${Boost_LIBRARIES})

