include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.16.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(footbsim_tests
    test_effective_stats.cpp
    test_event_probabilities.cpp
    test_match_context.cpp
    test_match_engine_smoke.cpp
)

target_link_libraries(footbsim_tests PRIVATE footbsim_core Catch2::Catch2WithMain)
footbsim_set_warnings(footbsim_tests)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(footbsim_tests)
