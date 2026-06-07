# ===========================================================================
# Adds the stb single-header libraries as a header-only INTERFACE library.
#
# stb headers are implementation-only: each header requires exactly one
# translation unit to define the implementation via a macro before including
# it. This target only provides the include path; each consuming target is
# responsible for defining the implementation macro in one of its own .cpp
# files, e.g.:
#
#   // stb_impl.cpp  (compiled once, in YOUR target)
#   #define STB_IMAGE_IMPLEMENTATION
#   #include <stb_image.h>
#
# Expects the submodule at: third_party/stb/
#
# Exposes:
#   stb        - INTERFACE library target
#   stb::stb   - namespaced alias
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Locate submodule
# ---------------------------------------------------------------------------

set(STB_DIR "${CMAKE_SOURCE_DIR}/third_party/stb")

foreach(_header IN ITEMS
  "${STB_DIR}/stb_image.h"
  "${STB_DIR}/stb_image_write.h"
  "${STB_DIR}/stb_image_resize2.h"
)
  if(NOT EXISTS "${_header}")
    message(FATAL_ERROR
      "[rs] add_stb: required header not found: '${_header}'.")
  endif()
endforeach()

# ---------------------------------------------------------------------------
# Interface library
# ---------------------------------------------------------------------------

add_library(stb INTERFACE)
add_library(stb::stb ALIAS stb)

target_include_directories(stb SYSTEM INTERFACE
  "${STB_DIR}"
)

# ---------------------------------------------------------------------------
# Engine-wide third-party setup (silence warnings, IDE folder, etc.)
# ---------------------------------------------------------------------------

rs_third_party_setup(stb)

set_target_properties(stb PROPERTIES
  FOLDER "ThirdParty/STB"
)

message(STATUS "[rs] stb configured (HEADER-ONLY) : (${STB_DIR})")
