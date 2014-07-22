cmake_minimum_required(VERSION 2.6)

project("sf2")

include_directories(AFTER include)

install(DIRECTORY include/sf2/ DESTINATION include/sf2 FILES_MATCHING PATTERN "*.h*")

add_executable("test01.bin" "tests/test01.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wextra -Wall")
