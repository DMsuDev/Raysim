# ===========================================================================
# Adds spdlog as a header-only INTERFACE library.
# spdlog is used as header-only (no pre-compiled core) to keep the build
# simple; if compile times become an issue, enable SPDLOG_COMPILED_LIB.
#
# Expects the submodule at: third_party/spdlog/
#   third_party/spdlog/include/
#
# Exposes:
#   spdlog        - INTERFACE library target
#   spdlog::spdlog - namespaced alias
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Locate submodule
# ---------------------------------------------------------------------------

set(SPDLOG_DIR "${CMAKE_SOURCE_DIR}/third_party/spdlog")

if(NOT EXISTS "${SPDLOG_DIR}/include")
  message(FATAL_ERROR
    "[rs] add_spdlog: include directory not found at '${SPDLOG_DIR}/include'.")
endif()

# ---------------------------------------------------------------------------
# Interface library
# ---------------------------------------------------------------------------

add_library(spdlog INTERFACE)
add_library(spdlog::spdlog ALIAS spdlog)

target_include_directories(spdlog SYSTEM INTERFACE
  "${SPDLOG_DIR}/include"
)

# spdlog's async features and the default thread-safe logger require threads.
target_link_libraries(spdlog INTERFACE Threads::Threads)

# ---------------------------------------------------------------------------
# Optional: compile-time configuration
# Uncomment to switch from header-only to pre-compiled mode (faster builds
# in large projects at the cost of adding a .cpp TU to the build).
# ---------------------------------------------------------------------------

# target_compile_definitions(spdlog INTERFACE SPDLOG_COMPILED_LIB)

# ---------------------------------------------------------------------------
# Engine-wide third-party setup (silence warnings, IDE folder, etc.)
# ---------------------------------------------------------------------------

rs_third_party_setup(spdlog)

set_target_properties(spdlog PROPERTIES
  FOLDER "ThirdParty/SPDLOG"
)

message(STATUS "[rs] spdlog configured (HEADER-ONLY) : (${SPDLOG_DIR})")
