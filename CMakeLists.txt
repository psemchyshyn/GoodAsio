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
#add_executable(select_server test-servers/select_server.cpp)
add_executable(async_server inc/my_time.h test-servers/TCPServer.h test-servers/TCPServer.cpp test-servers/TCPConnection.cpp test-servers/TCPConnection.h test-servers/asio_main.cpp)
#add_executable(sync_server sync_server.cpp my_time.h)
#add_executable(client client.cpp my_time.h)
#add_executable(raw_sync_serv rawc_serv.cpp)
add_executable(event_loop src/main.cpp inc/Event.hpp inc/SocketTemplate.h
        inc/EventQueue.h inc/EventQueue.h src/EventQueue.cpp
        inc/Sockets.h inc/EventLoop.h inc/IOEventContainer.h)
add_executable(client Clients/client.cpp)
add_executable(poll_server test-servers/poll_server.cpp)
add_executable(select_server test-servers/select_server)
add_executable(select-original test-servers/select-original.cpp)
# Linking
#target_link_libraries(sync_server ${Boost_LIBRARIES})
#target_link_libraries(async_server ${Boost_LIBRARIES} Threads::Threads)
#target_link_libraries(client ${Boost_LIBRARIES})