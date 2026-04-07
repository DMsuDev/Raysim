#!/usr/bin/env bash
# Backend environment setup script (Linux/macOS)

set -e  # Exit on error

echo ""
echo "========================================="
echo "   Python Environment Setup (Unix)"
echo "========================================="
echo ""

# Resolve project root (one level up from script)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

echo "Project root: $PROJECT_ROOT"

# Check Python availability
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 is not installed or not in PATH."
    exit 1
fi

VENV_PATH="$PROJECT_ROOT/.venv"

# Create virtual environment if missing
if [ ! -d "$VENV_PATH" ]; then
    echo "Creating virtual environment..."
    python3 -m venv "$VENV_PATH"
else
    echo "Virtual environment already exists"
fi

# Activate virtual environment
echo "Activating virtual environment..."
source "$VENV_PATH/bin/activate"

# Upgrade pip and install dependencies
echo "Installing dependencies..."
python -m pip install --upgrade pip pre-commit

echo ""
echo "Setup completed successfully!"
echo ""
echo "Next steps:"
echo "  Activate: source .venv/bin/activate"
echo ""
