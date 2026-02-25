cmake_minimum_required(VERSION 3.16)

# ─── Version settings ─────────────────────────────
set(RAYLIB_VERSION  5.5)
set(SPDLOG_VERSION  v1.17.0)

# ─── FetchContent module ──────────────────────────
include(FetchContent)
set(FETCHCONTENT_QUIET                ON  CACHE BOOL "Reduce verbosity during configure")
set(FETCHCONTENT_UPDATES_DISCONNECTED ON  CACHE BOOL "Skip git update on reconfigure (faster)")
set(FETCHCONTENT_BASE_DIR             "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Base directory for downloaded content")

# ─── SpdLog ────────────────────────────────────────
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG ${SPDLOG_VERSION} GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(spdlog)

# ─── RAYLIB ─────────────────────────────────────────
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG ${RAYLIB_VERSION} GIT_SHALLOW TRUE
)
set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build raylib examples when fetching" FORCE)

FetchContent_MakeAvailable(raylib)

# Note: we only fetch/make-available raylib here. Do not modify or link
# the top-level `${PROJECT_NAME}` from this vendor CMake file, the
# root CMakeLists.txt should decide how to link the fetched library.

# ─── Summary message ───────────────────────────────────────────────────────
message(STATUS "")
message(STATUS "Third-party dependencies summary:")
message(STATUS "  • raylib   : ${RAYLIB_VERSION}")
message(STATUS "  • spdlog   : ${SPDLOG_VERSION}")
message(STATUS "")
