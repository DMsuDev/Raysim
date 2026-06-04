#!/usr/bin/env bash
# _detect_triplet.sh - prints the default vcpkg triplet for the current platform.
# Sourced internally by other scripts; not meant to be run directly.
set -euo pipefail

OS="$(uname -s)"
ARCH="$(uname -m)"

case "$ARCH" in
    x86_64|amd64) ARCH_TAG="x64" ;;
    aarch64|arm64) ARCH_TAG="arm64" ;;
    *) ARCH_TAG="x64" ;;   # fallback
esac

case "$OS" in
    Linux)  echo "${ARCH_TAG}-linux" ;;
    Darwin) echo "${ARCH_TAG}-osx" ;;
    *)      echo "${ARCH_TAG}-linux" ;;
esac
