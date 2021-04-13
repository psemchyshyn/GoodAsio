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

#add_executable(sel Selector.cpp Selector.h)
#add_executable(select_server select_server.cpp)
#add_executable(async_server my_time.h TCPServer.h TCPServer.cpp TCPConnection.cpp TCPConnection.h main.cpp Event.cpp Event.h)
#add_executable(sync_server sync_server.cpp my_time.h)
#add_executable(client client.cpp my_time.h)
#add_executable(raw_sync_serv rawc_serv.cpp)
add_executable(event_loop src/main.cpp inc/Event.hpp inc/SocketTemplate.h src/SocketPull.cpp
        inc/SocketPull.h inc/EventQueue.h inc/EventQueue.h src/EventQueue.cpp
        inc/Sockets.h inc/EventLoop.h)

# Linking
#target_link_libraries(sync_server ${Boost_LIBRARIES})
#target_link_libraries(async_server ${Boost_LIBRARIES} Threads::Threads)
#target_link_libraries(client ${Boost_LIBRARIES})