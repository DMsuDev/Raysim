#!/usr/bin/env bash
# Bootstrap vcpkg and install project dependencies.
# Cross-platform (Linux/macOS). Called automatically by setup_all.sh.
# Used internally by CMake during configure phase.

set -euo pipefail

# Use $0 to get the script's directory, then navigate to the project root.
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
VCPKG_DIR="$ROOT_DIR/vcpkg"
BOOTSTRAP="$VCPKG_DIR/bootstrap-vcpkg.sh"

if [ ! -f "$BOOTSTRAP" ]; then
    echo "vcpkg not found. Cloning vcpkg submodule into $VCPKG_DIR..."
    if ! git submodule update --init --recursive; then
        echo "ERROR: Failed to initialize vcpkg submodule or other submodules. Check the output above for details."
        echo "This operation requires:"
        echo "  - An active Internet connection"
        echo "  - A git repository already initialized (git init) and with submodules configured"
        echo " < --- --- >"
        echo "Otherwise, you can manually clone the vcpkg repository into the 'vcpkg' directory at the project root."
        exit 1
    fi
fi

echo "Bootstrapping vcpkg..."
if ! bash "$VCPKG_DIR/bootstrap-vcpkg.sh" -disableMetrics; then
    echo "vcpkg bootstrap failed. Check the output above for details."
    exit 1
fi

echo "Installing dependencies from vcpkg.json..."
if ! "$VCPKG_DIR/vcpkg" install --clean-after-build; then
    echo "vcpkg install failed. Check the output above for details."
    exit 1
fi

echo "vcpkg ready."
