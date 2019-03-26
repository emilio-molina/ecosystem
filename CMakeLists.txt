cmake_minimum_required(VERSION 3.0)

# Definitions
set(PROJECT_NAME_STR ecosystem)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Project title and define versioning
project(${PROJECT_NAME_STR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Dependencies
find_package(Boost COMPONENTS filesystem system iostreams REQUIRED)

# Assign the include directories
include_directories(${Boost_INCLUDE_DIRS})

# Gather all sources except the main entry point
file(GLOB_RECURSE INC_FILES src/cpp/*.hpp)
file(GLOB_RECURSE SRC_FILES src/cpp/*.cpp)

# Build
add_executable(ecosystem ${INC_FILES} ${SRC_FILES})
target_link_libraries(ecosystem ${Boost_LIBRARIES})
