cmake_minimum_required(VERSION 2.6)

project("sf2")

include_directories(AFTER include)

install(DIRECTORY include/sf2/ DESTINATION include/sf2 FILES_MATCHING PATTERN "*.h*")

FILE(GLOB_RECURSE INC  "include/*.h*")

add_executable("test01.bin"
    "tests/test01.cpp"
    ${INC}
)

add_executable("test_complex.bin"
    "tests/test_complex.cpp"
    ${INC}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wextra -Wall")
