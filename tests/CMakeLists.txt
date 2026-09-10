add_executable(footbsim_tests
    test_main.cpp
    test_effective_stats.cpp
    test_event_probabilities.cpp
    test_match_engine_smoke.cpp
)

target_link_libraries(footbsim_tests PRIVATE footbsim_core)
target_include_directories(footbsim_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
footbsim_set_warnings(footbsim_tests)

add_test(NAME footbsim_tests COMMAND footbsim_tests)
