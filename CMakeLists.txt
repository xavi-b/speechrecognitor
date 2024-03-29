cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME speechrecognitor)

project(${PROJECT_NAME})

# C++
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)
set(CMAKE_CXX_FLAGS                 "-Wall -Wextra -Wpedantic -pthread")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
    add_definitions(-DDEBUG)
else()
    message("RELEASE")
endif()

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
