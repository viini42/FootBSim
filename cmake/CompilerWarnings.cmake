function(footbsim_set_warnings target)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wshadow>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
endfunction()
