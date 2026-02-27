# ─── Version settings ─────────────────────────────
set(RAYLIB_VERSION  5.5)
set(SPDLOG_VERSION  1.17.0)

# ─── FetchContent module ──────────────────────────
include(FetchContent)
set(FETCHCONTENT_QUIET                ON  CACHE BOOL "Reduce verbosity during configure")
set(FETCHCONTENT_UPDATES_DISCONNECTED ON  CACHE BOOL "Skip git update on reconfigure (faster)")
set(FETCHCONTENT_BASE_DIR             "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Base directory for downloaded content")

# ─── SpdLog ────────────────────────────────────────
find_package(raylib ${RAYLIB_VERSION} CONFIG QUIET)
if(NOT TARGET raylib::raylib)
    message(STATUS "raylib not found, fetching via FetchContent")
    set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build raylib examples when fetching" FORCE)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG ${RAYLIB_VERSION}
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib)
else()
    message(STATUS "Using system raylib")
endif()

# ─── spdlog ──────────────────────────────────────
find_package(spdlog ${SPDLOG_VERSION} CONFIG QUIET)
if(NOT TARGET spdlog::spdlog)
    message(STATUS "spdlog not found, fetching via FetchContent")
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v${SPDLOG_VERSION}
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(spdlog)
else()
    message(STATUS "Using system spdlog")
endif()

# ─── Summary message ───────────────────────────────────────────────────────
message(STATUS "")
message(STATUS "Third-party dependencies summary:")
message(STATUS "  • raylib   : ${RAYLIB_VERSION}")
message(STATUS "  • spdlog   : ${SPDLOG_VERSION}")
message(STATUS "")
