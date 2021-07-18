PROJECT(example)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-m64")
add_definitions("-Wall -std=c++17")

INCLUDE_DIRECTORIES(src/)
LINK_DIRECTORIES(src/)

ADD_EXECUTABLE(example example.cpp)

TARGET_LINK_LIBRARIES(example)