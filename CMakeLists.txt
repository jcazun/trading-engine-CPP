cmake_minimum_required(VERSION 3.10)
project(TradingEngine)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Get status
message(STATUS "Sources found: ${SOURCES}")

# Create executable
add_executable(trading_engine ${SOURCES})

target_link_libraries(trading_engine PRIVATE sqlite3)