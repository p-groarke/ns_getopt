cmake_minimum_required(VERSION 3.10.0)
project(ns_getopt VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/include/ns_getopt/ns_getopt.h")
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${HEADER_FILES}>
)
target_include_directories(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set(CMAKE_CXX_STANDARD 17)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra -Wpedantic -Werror)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra -Wpedantic -Werror)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(${PROJECT_NAME} INTERFACE /W4 /WX)

endif()

# Install Package Configuration
set(NS_GETOPT_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/ns_getopt")
install(FILES ${HEADER_FILES} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/ns_getopt")

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/ns_getoptConfigVersion.cmake"
	VERSION ${${PROJECT_NAME}_VERSION}
	COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
	cmake/ns_getoptConfig.cmake.in
	${NS_GETOPT_INSTALL_CONFIGDIR}/ns_getoptConfig.cmake
	INSTALL_DESTINATION ${NS_GETOPT_INSTALL_CONFIGDIR}
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${NS_GETOPT_INSTALL_CONFIGDIR}/ns_getoptConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/ns_getoptConfigVersion.cmake"
	DESTINATION ${NS_GETOPT_INSTALL_CONFIGDIR}
)

install(TARGETS ${PROJECT_NAME} EXPORT ns_getoptTargets)

export(EXPORT ns_getoptTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/ns_getoptTargets.cmake"
)

install(
	EXPORT ns_getoptTargets FILE ns_getoptTargets.cmake
	DESTINATION ${NS_GETOPT_INSTALL_CONFIGDIR}
)

export(PACKAGE ${PROJECT_NAME})

# Examples
option(BUILD_EXAMPLES "Build accompanying examples." On)

if (${BUILD_EXAMPLES})
	add_executable(${PROJECT_NAME}_example example/ns_getopt_example.cpp)
	target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})

	add_executable(custom_buffer example/custom_buffer.cpp)
	target_link_libraries(custom_buffer ${PROJECT_NAME})
endif()