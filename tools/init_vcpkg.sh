#!/usr/bin/env bash
set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VCPKG_DIR="$ROOT_DIR/vcpkg"

echo "Bootstrapping vcpkg..."
"$VCPKG_DIR/bootstrap-vcpkg.sh"

echo "Installing dependencies from vcpkg.json..."
"$VCPKG_DIR/vcpkg" install --clean-after-build

echo "vcpkg initialized!"
