#!/usr/bin/env bash
# =============================================================================
# Raysim - build.sh
# Thin wrapper around cmake --preset for convenience.
#
# Common presets (CMakePresets.json):
#   debug            Debug + sanitizers    -> build/ninja/debug
#   release          Release + LTO         -> build/ninja/release
#   relwithdebinfo   Release with symbols  -> build/ninja/relwithdebinfo
#
# User presets (CMakeUserPresets.json):
#   local-ninja-debug      local-ninja-release      local-ninja-sanitized
#
# Usage: ./tools/build.sh [<preset>] [--clean] [--config-only]
#   default preset: debug
# =============================================================================
set -euo pipefail

CYAN='\033[0;36m'; GREEN='\033[0;32m'; RED='\033[0;31m'; RESET='\033[0m'
info() { echo -e "${CYAN}[build]${RESET} $*"; }
die()  { echo -e "${RED}[build] ERROR:${RESET} $*" >&2; exit 1; }

PRESET="debug"
CLEAN=0
CONFIG_ONLY=0

# =============================================================================
# Argument parsing
# =============================================================================

for arg in "$@"; do
    case "$arg" in
        --clean)        CLEAN=1 ;;
        --config-only)  CONFIG_ONLY=1 ;;
        --help|-h)
            echo "Usage: $0 [<preset>] [--clean] [--config-only]"
            echo "  default preset: debug"
            exit 0 ;;
        --*) die "Unknown option: $arg" ;;
        *)   PRESET="$arg" ;;
    esac
done

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

# =============================================================================
# Preset -> build directory mapping
# =============================================================================
# Used only for --clean; cmake --preset derives binaryDir from the preset JSON.

preset_build_dir() {
    local p="$1"
    if [[ "$p" == *"msvc"* ]];            then echo "build/msvc"; return; fi
    if [[ "$p" == *"relwithdebinfo"* ]];  then echo "build/ninja/relwithdebinfo"; return; fi
    if [[ "$p" == *"release"* ]];         then echo "build/ninja/release"; return; fi
    if [[ "$p" == *"debug"* ]] || [[ "$p" == *"sanitized"* ]]; then
        echo "build/ninja/debug"; return
    fi
    echo "build/$p"
}

# =============================================================================
# Clean (optional)
# =============================================================================

if [[ $CLEAN -eq 1 ]]; then
    BUILD_DIR="$(preset_build_dir "$PRESET")"
    if [[ -d "$BUILD_DIR" ]]; then
        info "Cleaning $BUILD_DIR..."
        rm -rf "$BUILD_DIR"
    else
        info "Nothing to clean - build directory does not exist."
    fi
fi

# =============================================================================
# Configure
# =============================================================================

info "Configuring preset: ${PRESET}..."
cmake --preset "$PRESET"

if [[ $CONFIG_ONLY -eq 1 ]]; then
    echo -e "\n${GREEN}[build] Configure-only mode - skipping build step.${RESET}\n"
    exit 0
fi

# =============================================================================
# Build
# =============================================================================

info "Building preset: ${PRESET}..."
cmake --build --preset "$PRESET"

echo -e "\n${GREEN}[build] Done - preset: ${PRESET}${RESET}\n"
