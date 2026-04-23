#!/usr/bin/env bash
set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLS_DIR="$ROOT_DIR/tools"

echo "Setting up Python environment..."
python3 "$TOOLS_DIR/setup_python_env.py"

echo "Installing pre-commit hooks..."
python3 "$TOOLS_DIR/install_precommit.py"

echo "Initializing vcpkg..."
bash "$TOOLS_DIR/init_vcpkg.sh"

echo "All tools installed successfully!"
