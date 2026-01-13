cmake_minimum_required(VERSION 3.10)
project(chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set path to ASIO headers
include_directories("C:/Users/rjeev/Documents/library/asio-1.30.2/include")

# Fix Windows target version warning
add_definitions(-D_WIN32_WINNT=0x0601)

# Executables
add_executable(server server.cpp)
add_executable(client client.cpp)

# Link required libraries
target_link_libraries(server
    ws2_32
    mswsock
)

target_link_libraries(client
    ws2_32
)

#target_link_libraries(server ws2_32)
#target_link_libraries(client ws2_32)

