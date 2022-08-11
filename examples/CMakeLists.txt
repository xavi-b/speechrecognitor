set(MODULE_NAME speechrecognitorexample)

message("Compiling \"${MODULE_NAME}\"")

add_executable(${MODULE_NAME} "")

# dependencies
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

target_include_directories(${MODULE_NAME}
PUBLIC
    ${PROJECT_SOURCE_DIR}/lib/include
)

target_link_libraries(${MODULE_NAME}
PUBLIC
    Qt5::Core
    Qt5::Widgets
    speechrecognitor
)

# sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(${MODULE_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
