cmake_minimum_required(VERSION 3.25)
project(FootbSim CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CompilerWarnings.cmake)

option(FOOTBSIM_BUILD_TESTS "Build FootbSim unit tests" ON)

add_subdirectory(src)
