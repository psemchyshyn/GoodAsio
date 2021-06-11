include_directories(${PROJECT_SOURCE_DIR}/lib/inc)
add_library(GoodASIO SHARED src/IOEventContainer.cpp src/EventQueue.cpp src/Event.cpp
        src/EventLoop.cpp src/IOManager.cpp src/Sockets.cpp src/SocketTemplate.cpp inc/TimeFunctions.h)