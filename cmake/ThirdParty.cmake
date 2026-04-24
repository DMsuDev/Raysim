include_guard()

# ============================================================
# THIRD PARTY LIBRARIES
# ============================================================

# Extend module search path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# Load third-party modules
include(add_glad)
include(add_imgui)
include(add_spdlog)
