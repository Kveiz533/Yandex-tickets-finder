cmake_minimum_required(VERSION 3.14)
project(lab6)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(workAPI)
add_subdirectory(cpr REQUIRED)
add_subdirectory(json)
add_subdirectory(user)
add_subdirectory(parser)
add_subdirectory(bin)

