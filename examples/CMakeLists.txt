set(EXAMPLES_LIST "")

function(create_example TARGET SOURCE)
    # Create an executable target with the specified name and source file
    add_executable(${TARGET} ${SOURCE})

    # Link the target against the RaySim library
    target_link_libraries(${TARGET} PRIVATE Raysim)

    # Set the output directory for the executable
    set_target_properties(${TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    # Collect example targets so we can copy resources once for all
    list(APPEND EXAMPLES_LIST ${TARGET})
endfunction()

create_example(Mouse_detection_2D Mouse_detection_2D/main.cpp)
create_example(Bouncing_Balls Bouncing_Balls/main.cpp)
create_example(Lissajous_Curves Lissajous_Curves/main.cpp)

# Copy resources once for all examples (they share the same output dir)
add_custom_target(copy_examples_resources ALL
    DEPENDS ${EXAMPLES_LIST}
)

add_custom_command(TARGET copy_examples_resources POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/resources
    COMMENT "Copying resources for all examples"
)
