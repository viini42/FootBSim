add_library(footbsim_core STATIC
    match_context.cpp
    match_state.cpp
    rng.cpp
    match_engine.cpp
)

target_include_directories(footbsim_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(footbsim_core PUBLIC cxx_std_23)
footbsim_set_warnings(footbsim_core)
