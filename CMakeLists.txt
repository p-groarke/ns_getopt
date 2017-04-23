cmake_minimum_required(VERSION 3.8.0)
set(PROJECT_NAME notshit_getopt)
project(${PROJECT_NAME})

file(GLOB_RECURSE HEADER_FILES "include/notshit_getopt.h")
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} INTERFACE include/)

file(GLOB_RECURSE SRC "example/*.cpp" "example/*.h")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
add_executable(${PROJECT_NAME}_example ${SRC})
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})

# install(FILES ${SRC} DESTINATION /usr/local/include/bench_util/)

# Examples
# file(GLOB EX_ALL RELATIVE ${PROJECT_SOURCE_DIR}/examples/ ${PROJECT_SOURCE_DIR}/examples/*)
# set(EX_DIRS "")

# file(GLOB EX_SRC ${PROJECT_SOURCE_DIR}/examples/*.cpp)

# foreach(d ${EX_ALL})
# 	if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/examples/${d})
# 		message(STATUS "Found example : " ${d})

# 		file(GLOB D_SRC RELATIVE ${PROJECT_SOURCE_DIR}/examples/${d}/ ${PROJECT_SOURCE_DIR}/examples/${d}/*.cpp)
# 		foreach(f ${D_SRC})
# 			string(REPLACE "main" "" n ${f})
# 			string(REPLACE ".cpp" "" n ${n})

# 			add_executable(${d}${n} ${PROJECT_SOURCE_DIR}/examples/${d}/${f})
# 		endforeach()
# 	endif()
# endforeach()
