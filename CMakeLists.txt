cmake_minimum_required(VERSION 2.6)

project(sf2)

include_directories(AFTER include)

install(DIRECTORY include/sf2/ DESTINATION include/sf2 FILES_MATCHING PATTERN "*.h*")

option(SF2_BUILD_TESTS "Build tests" off)

if(SF2_BUILD_TESTS)
	enable_testing()

	MESSAGE("Building tests for sf2")

	FILE(GLOB_RECURSE INC  "include/*.h*")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wextra -Wall")

	add_executable(test_simple
		"tests/test_simple.cpp"
		${INC}
	)
	add_executable(test_advanced
		"tests/test_advanced.cpp"
		${INC}
	)

	add_test(NAME "SF2 simple usage test"   COMMAND test_simple)
	add_test(NAME "SF2 advanced usage test" COMMAND test_advanced)
endif()
