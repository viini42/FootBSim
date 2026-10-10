# Standalone team generator: turns season-total CSVs (this folder) into the
# TeamStats values the engine consumes. Deliberately independent of the main
# FootbSim project -- build it on its own:
#
#   cmake -S data -B data/build -G Ninja && cmake --build data/build
#   data/build/footbsim_team_stats_tool data/example_teams.csv
cmake_minimum_required(VERSION 3.25)
project(FootbSimTeamStatsTool CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TEAM_STATS_TOOL_BUILD_TESTS "Build the team stats tool unit tests" ON)

set(TEAM_STATS_TOOL_WARNINGS
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wshadow>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_library(team_stats_deriver STATIC team_stats_deriver.cpp)
target_include_directories(team_stats_deriver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(team_stats_deriver PRIVATE ${TEAM_STATS_TOOL_WARNINGS})

add_executable(footbsim_team_stats_tool main.cpp)
target_link_libraries(footbsim_team_stats_tool PRIVATE team_stats_deriver)
target_compile_options(footbsim_team_stats_tool PRIVATE ${TEAM_STATS_TOOL_WARNINGS})

if(TEAM_STATS_TOOL_BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.16.0
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(team_stats_tool_tests tests/test_team_stats_deriver.cpp)
    target_link_libraries(team_stats_tool_tests PRIVATE team_stats_deriver Catch2::Catch2WithMain)
    target_compile_options(team_stats_tool_tests PRIVATE ${TEAM_STATS_TOOL_WARNINGS})

    list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests(team_stats_tool_tests)
endif()
