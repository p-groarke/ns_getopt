include (ExternalProject)

ExternalProject_Add(catch2_ext
	GIT_REPOSITORY https://github.com/catchorg/Catch2
	GIT_TAG v2.1.2
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release #${CMAKE_ARGS}
		-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
		-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}
		-DCATCH_BUILD_EXAMPLES=OFF
		-DBUILD_TESTING=OFF
	UPDATE_COMMAND ""
)

add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)
add_dependencies(catch2 catch2_ext)

add_executable(tests "tests.cpp")
add_test(NAME tests COMMAND tests)
target_link_libraries(tests PRIVATE catch2)
add_dependencies(tests ${PROJECT_NAME})

set(VERBOSE)
if (${TESTING_VERBOSE})
	set(VERBOSE "--verbose")
endif()

add_custom_command(
	TARGET tests
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
		${CMAKE_CTEST_COMMAND} ${VERBOSE} -C $<CONFIGURATION>
)
