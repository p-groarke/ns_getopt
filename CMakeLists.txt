cmake_minimum_required(VERSION 3.8.0)
set(PROJECT_NAME ns_getopt)
project(${PROJECT_NAME})

file(GLOB_RECURSE HEADER_FILES "include/ns_getopt.h")
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} INTERFACE include/)

file(GLOB_RECURSE SRC "example/*.cpp" "example/*.h")
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wpedantic ${CMAKE_CXX_FLAGS}")
add_executable(${PROJECT_NAME}_example ${SRC})
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})

