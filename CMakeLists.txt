project(serial-port)
cmake_minimum_required(VERSION 3.14)

add_library(serial-port)
target_include_directories(serial-port PUBLIC .)
target_sources(serial-port PRIVATE serialPort.cpp)
target_compile_features(serial-port PUBLIC cxx_std_20)

add_executable(test)
target_sources(test PRIVATE main.cpp)
target_link_libraries(test PRIVATE serial-port)

