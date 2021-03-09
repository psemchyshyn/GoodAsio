cmake_minimum_required(VERSION 3.16)
project(echo_server)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(ws2_32 wsock32)


#add_executable(echo-server main.cpp)
add_executable(server server.cpp)
target_link_libraries(server ${Boost_LIBRARIES})
add_executable(client client.cpp)
target_link_libraries(client ${Boost_LIBRARIES})