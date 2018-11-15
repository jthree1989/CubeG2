cmake_minimum_required(VERSION 3.4.1)
project(CubeG2)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB SRC *.cpp)

add_library(${PROJECT_NAME}
    SHARED
    ${SRC}
)

target_link_libraries( ${PROJECT_NAME}
    log
    GLESv2
)