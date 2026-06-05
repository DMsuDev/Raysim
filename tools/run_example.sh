#!/usr/bin/env bash
# =============================================================================
# Raysim - run_example.sh
# Finds and launches a compiled example binary by name.
#
# Binary locations (from CMakePresets.json binaryDir + RS output structure):
#   Ninja presets  -> build/ninja/<preset>/bin/<Name>
#   MSVC presets   -> build/msvc/<Debug|Release>/bin/<Name>
#
# Usage: ./tools/run_example.sh <ExampleName> [<preset>]
# Example:
#   ./tools/run_example.sh BouncingBalls release
# =============================================================================
set -euo pipefail

CYAN='\033[0;36m'; RED='\033[0;31m'; RESET='\033[0m'
info() { echo -e "${CYAN}[run]${RESET} $*"; }
die()  { echo -e "${RED}[run] ERROR:${RESET} $*" >&2; exit 1; }

EXAMPLE_NAME="${1:-}"
PRESET="${2:-debug}"

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# =============================================================================
# Show available examples when no name is provided
# =============================================================================

if [[ -z "$EXAMPLE_NAME" ]]; then
    echo "Usage: $0 <ExampleName> [<preset>]"
    echo ""
    echo "Available examples:"
    find "$REPO_ROOT/examples" -mindepth 1 -maxdepth 1 -type d \
        ! -name shared -exec basename {} \; 2>/dev/null || echo "  (none found)"
    exit 1
fi

# =============================================================================
# Preset -> build directory mapping
# =============================================================================

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
# Locate binary
# =============================================================================

BUILD_DIR="$REPO_ROOT/$(preset_build_dir "$PRESET")"

if [[ ! -d "$BUILD_DIR" ]]; then
    die "Build directory not found: $BUILD_DIR\n  Run: ./tools/build.sh $PRESET"
fi

# Search under bin/ subdirectories inside the build dir
BINARY="$(find "$BUILD_DIR" -path "*/bin/$EXAMPLE_NAME" -type f 2>/dev/null | head -1)"

if [[ -z "$BINARY" ]]; then
    die "Binary '$EXAMPLE_NAME' not found under $BUILD_DIR.\n  Build first: ./tools/build.sh $PRESET"
fi

# =============================================================================
# Launch
# =============================================================================

info "Running: $BINARY"
exec "$BINARY"
