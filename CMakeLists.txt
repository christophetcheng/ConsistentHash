cmake_minimum_required(VERSION 3.7)

project(ConsistentHash)
include_directories (src)
include_directories (src/consistenthash)
file(GLOB SOURCES "src/*.cpp" "src/consistenthash/*.h" "src/consistenthash/*.cpp")

add_executable(consistenthash ${SOURCES})
