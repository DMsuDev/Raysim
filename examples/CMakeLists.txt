if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

# Destination for shared assets. If the generator is multi-config, place
# assets under a config subfolder (e.g. Debug/Release) using $<CONFIG>.
if(CMAKE_CONFIGURATION_TYPES)
    set(ASSETS_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/assets")
else()
    set(ASSETS_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
endif()

add_custom_target(SharedAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/examples/shared/assets
        ${ASSETS_DEST}
    COMMENT "Copying shared assets for examples"
)

function(add_example TARGET SOURCE)
    add_executable(${TARGET} ${SOURCE})
    target_link_libraries(${TARGET} PRIVATE Raysim)
    set_target_properties(${TARGET} PROPERTIES FOLDER "Examples")
    add_dependencies(${TARGET} SharedAssets)
endfunction()

add_subdirectory(BouncingBalls)
add_subdirectory(LissajousCurves)
add_subdirectory(Mouse2D)
