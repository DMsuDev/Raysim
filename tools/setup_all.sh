#!/usr/bin/env bash
# =============================================================================
# Raysim - setup_all.sh
# Bootstraps vcpkg and installs pre-commit hooks (no Python venv needed).
# Usage: ./tools/setup_all.sh [--skip-vcpkg] [--skip-precommit]
# =============================================================================
set -euo pipefail

# =============================================================================
# Colors
# =============================================================================

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; CYAN='\033[0;36m'
BOLD='\033[1m'; RESET='\033[0m'

info()    { echo -e "${CYAN}[setup]${RESET} $*"; }
success() { echo -e "${GREEN}[setup]${RESET} $*"; }
warn()    { echo -e "${YELLOW}[setup]${RESET} $*"; }
die()     { echo -e "${RED}[setup] ERROR:${RESET} $*" >&2; exit 1; }

# =============================================================================
# Argument parsing
# =============================================================================

SKIP_VCPKG=0
SKIP_PRECOMMIT=0
for arg in "$@"; do
    case "$arg" in
        --skip-vcpkg)      SKIP_VCPKG=1 ;;
        --skip-precommit)  SKIP_PRECOMMIT=1 ;;
        --help|-h)
            echo "Usage: $0 [--skip-vcpkg] [--skip-precommit]"
            exit 0 ;;
        *) die "Unknown argument: $arg" ;;
    esac
done

# =============================================================================
# Repo root
# =============================================================================

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

echo -e "\n${BOLD}=== Raysim Setup ===${RESET}\n"

# =============================================================================
# Git submodules
# =============================================================================

if [[ ! -f "vcpkg/bootstrap-vcpkg.sh" ]]; then
    info "Initialising git submodules..."
    git submodule update --init --recursive
fi

# =============================================================================
# vcpkg bootstrap and dependency install
# =============================================================================

if [[ $SKIP_VCPKG -eq 0 ]]; then
    info "Bootstrapping vcpkg..."
    bash vcpkg/bootstrap-vcpkg.sh -disableMetrics

    TRIPLET="$(bash tools/_detect_triplet.sh)"
    info "Installing vcpkg dependencies for triplet: $TRIPLET (this may take a while on first run)..."
    ./vcpkg/vcpkg install --triplet "$TRIPLET"
    success "vcpkg ready."
else
    warn "Skipping vcpkg (--skip-vcpkg)."
fi

# =============================================================================
# pre-commit hooks
# =============================================================================
# pre-commit is a Python tool but does NOT require a project-local venv.
# Install it once globally: pip install pre-commit  OR  pipx install pre-commit

if [[ $SKIP_PRECOMMIT -eq 0 ]]; then
    if ! command -v pre-commit &>/dev/null; then
        warn "pre-commit not found. Attempting global install via pip..."
        if command -v pipx &>/dev/null; then
            pipx install pre-commit
        elif command -v pip3 &>/dev/null; then
            pip3 install --user pre-commit
        elif command -v pip &>/dev/null; then
            pip install --user pre-commit
        else
            die "Cannot install pre-commit: pip/pipx not found. Install it manually:\n  pip install pre-commit"
        fi
    fi
    info "Installing pre-commit hooks..."
    pre-commit install
    success "pre-commit hooks installed."
else
    warn "Skipping pre-commit (--skip-precommit)."
fi

# =============================================================================
# Done
# =============================================================================

echo -e "\n${GREEN}${BOLD}Setup complete!${RESET}"
echo -e "Next steps:"
echo -e "  cmake --preset debug          # configure"
echo -e "  cmake --build --preset debug  # build"
echo -e "  # or use the helper:"
echo -e "  ./tools/build.sh debug\n"
