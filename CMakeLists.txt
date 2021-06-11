cmake_minimum_required(VERSION 3.16)
project(echo_server)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(Threads REQUIRED)

#OS Specifics
if (WIN32)
    link_libraries(ws2_32 wsock32)
endif (WIN32)

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()

# Add sub-CMake for making
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)

# Adding Executables
#add_executable(sel Selector.cpp Selector.h)
#add_executable(select_server test-servers/select_server.cpp)
add_executable(async_server lib/inc/TimeMeasurement.h test-servers/TCPServer.h
        test-servers/TCPServer.cpp test-servers/TCPConnection.cpp test-servers/TCPConnection.h test-servers/asio_main.cpp )
#add_executable(sync_server sync_server.cpp my_time.h)
#add_executable(client client.cpp my_time.h)
add_executable(raw_sync_serv test-servers/raw_sync_serv.cpp)
add_executable(event_loop main.cpp timers-test.cpp)
add_executable(client Clients/client.cpp)
add_executable(poll_server test-servers/poll_server.cpp)
add_executable(select-original test-servers/select-original.cpp)
add_executable(multithreaded test-servers/multithreaded.cpp)
add_executable(epoll_server test-servers/epoll_server.cpp)
add_executable(multithreaded_poll_server test-servers/multithreaded_poll_server.cpp)
add_executable(multithreaded_epoll_server test-servers/multithreaded_epoll_server.cpp)
add_executable(timers-test timers-test.cpp)
# Linking
#target_link_libraries(sync_server ${Boost_LIBRARIES})
target_link_libraries(multithreaded_poll_server Threads::Threads)
target_link_libraries(multithreaded_epoll_server Threads::Threads)
target_link_libraries(multithreaded Threads::Threads)
target_link_libraries(async_server ${Boost_LIBRARIES} Threads::Threads)
target_link_libraries(event_loop Threads::Threads GoodASIO)
target_link_libraries(timers-test GoodASIO)
#target_link_libraries(client ${Boost_LIBRARIES})