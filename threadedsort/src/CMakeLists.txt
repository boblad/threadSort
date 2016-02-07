set(CMAKE_CXX_FLAGS "-std=c++14 -std=c++11 -Wall -pthread")
set(srcs main.cpp)
add_executable(threadSort ${srcs})
