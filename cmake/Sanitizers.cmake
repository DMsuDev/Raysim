if(ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(SANITIZER_FLAGS
            -fsanitize=address
            -fsanitize=undefined
            -fsanitize=leak            # Optional: LeakSanitizer (detect memory leaks)
            -fno-sanitize-recover=all
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls
        )
        add_compile_options(${SANITIZER_FLAGS})
        add_link_options(${SANITIZER_FLAGS})

        message(STATUS "Sanitizers enabled: Address + Undefined Behavior + Leak")

    elseif(MSVC)
        # AddressSanitizer in MSVC (VS 2019+)
        add_compile_options(/fsanitize=address)
        add_link_options(/fsanitize=address)
        message(STATUS "AddressSanitizer enabled (MSVC)")
    endif()
endif()
