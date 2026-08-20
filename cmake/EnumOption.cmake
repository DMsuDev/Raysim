# ==============================================================================
#  RAYSIM - ENUM OPTION
# ==============================================================================
#  Description: Provides enum_option(): a macro for defining CMake cache
#               variables restricted to a closed set of valid values, with
#               automatic default selection and IDE dropdown support.
#
#  Adapted from raylib (https://github.com/raysan5/raylib)
#  Original authors: Ramon Santamaria (@raysan5) and contributors.
#  Licensed under the zlib License.
#
#  Modifications by @DMsuDev (2026):
#    - Added include_guard()
#    - Replaced generic 'default' variable with '_enum_default' to avoid
#      scope collisions inside macros
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the Apache License, Version 2.0.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

include_guard()

# ------------------------------------------------------------------------------
#  Public API
# ------------------------------------------------------------------------------
#  enum_option(<var> <values> <description>)
#
#  Creates a CACHE STRING variable <var> restricted to <values>.
#  The first element of <values> is used as the default.
#  Exposes the allowed values as a STRINGS property for IDE dropdowns.
#
#  Example:
#    enum_option(RS_WINDOWING_BACKEND "glfw;sdl2" "Windowing backend to use")
# ------------------------------------------------------------------------------
macro(enum_option var values description)
  set(${var}_VALUES ${values})
  list(GET ${var}_VALUES 0 _enum_default)

  if(NOT DEFINED ${var})
    set(${var} "${_enum_default}" CACHE STRING "${description}")
  endif()

  set_property(CACHE ${var} PROPERTY STRINGS ${${var}_VALUES})

  if(NOT ";${${var}_VALUES};" MATCHES ";${${var}};")
    message(FATAL_ERROR
      "[EnumOption] Invalid value '${${var}}' for ${var}.\n"
      "  Allowed: -D${var}=${${var}_VALUES}")
  endif()
endmacro()
