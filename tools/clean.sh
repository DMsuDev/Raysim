#!/usr/bin/env bash
# =============================================================================
# Raysim - clean.sh
# Removes build artefacts produced by CMake presets.
#
# Preset -> build directory:
#   debug          -> build/ninja/debug
#   release        -> build/ninja/release
#   relwithdebinfo -> build/ninja/relwithdebinfo
#   msvc           -> build/msvc      (all MSVC configurations)
#   ninja          -> build/ninja     (all Ninja configurations)
#   all            -> all of the above + vcpkg/buildtrees  [default]
#
# Usage: ./tools/clean.sh [debug|release|relwithdebinfo|msvc|ninja|all]
# =============================================================================
set -euo pipefail

YELLOW='\033[1;33m'; GREEN='\033[0;32m'; GRAY='\033[0;90m'; RESET='\033[0m'
info()    { echo -e "${YELLOW}[clean]${RESET} $*"; }
skipped() { echo -e "${GRAY}[clean]${RESET} $*"; }

TARGET="${1:-all}"
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

# =============================================================================
# Helpers
# =============================================================================

remove_dir() {
    local path="$1"
    if [[ -d "$path" ]]; then
        info "Removing $path..."
        rm -rf "$path"
    else
        skipped "$path does not exist, skipping."
    fi
}

# =============================================================================
# Clean
# =============================================================================

case "$TARGET" in
    debug)          remove_dir build/ninja/debug ;;
    release)        remove_dir build/ninja/release ;;
    relwithdebinfo) remove_dir build/ninja/relwithdebinfo ;;
    msvc)           remove_dir build/msvc ;;
    ninja)          remove_dir build/ninja ;;
    all)
        remove_dir build/ninja
        remove_dir build/msvc
        if [[ -d "vcpkg/buildtrees" ]]; then
            info "Removing vcpkg/buildtrees..."
            rm -rf vcpkg/buildtrees
        fi
        ;;
    --help|-h)
        echo "Usage: $0 [debug|release|relwithdebinfo|msvc|ninja|all]"
        exit 0 ;;
    *)
        echo "Unknown target: $TARGET" >&2
        echo "Valid targets: debug release relwithdebinfo msvc ninja all" >&2
        exit 1 ;;
esac

echo -e "${GREEN}[clean] Done.${RESET}"
