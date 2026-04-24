include_guard()

# Path to spdlog
set(SPDLOG_DIR ${CMAKE_SOURCE_DIR}/third_party/spdlog)

# ============================================================
# spdlog (header-only)
# ============================================================
add_library(spdlog INTERFACE)

target_include_directories(spdlog
    SYSTEM INTERFACE
    ${SPDLOG_DIR}/include
)

# Disable warnings for consumers
disable_warnings(spdlog)

# Group in IDEs
set_target_properties(spdlog PROPERTIES FOLDER "third_party")
