#!/usr/bin/env bash
# =============================================================================
# Raysim - check_deps.sh
# Verifies that all required and optional build tools are available on PATH.
# Exits 0 if all REQUIRED tools are found, 1 if any are missing.
# Optional tools report their status but do not affect the exit code.
# =============================================================================
set -euo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; CYAN='\033[0;36m'
YELLOW='\033[1;33m'; GRAY='\033[0;90m'; BOLD='\033[1m'; RESET='\033[0m'

MISSING=0

# =============================================================================
# Helpers
# =============================================================================

# check_required: prints OK/FAIL and sets MISSING=1 on failure.
check_required() {
    local cmd="$1"
    local label="${2:-$1}"
    local hint="${3:-}"
    if command -v "$cmd" &>/dev/null; then
        local ver
        ver="$("$cmd" --version 2>&1 | head -1)" || ver="(unknown version)"
        echo -e "  ${GREEN}[OK]${RESET} ${label}: ${ver}"
    else
        echo -e "  ${RED}[!!]${RESET} ${label}: NOT FOUND"
        [[ -n "$hint" ]] && echo -e "       ${YELLOW}-> $hint${RESET}"
        MISSING=1
    fi
}

# check_optional: prints status but never sets MISSING.
check_optional() {
    local cmd="$1"
    local label="${2:-$1}"
    local hint="${3:-}"
    if command -v "$cmd" &>/dev/null; then
        local ver
        ver="$("$cmd" --version 2>&1 | head -1)" || ver="(unknown version)"
        echo -e "  ${CYAN}[ok]${RESET} ${label}: ${ver}"
    else
        echo -e "  ${GRAY}[--]${RESET} ${label}: not found"
        [[ -n "$hint" ]] && echo -e "       ${GRAY}-> $hint${RESET}"
    fi
}

# =============================================================================
# Required tools
# =============================================================================

echo -e "\n${BOLD}=== Raysim Dependency Check ===${RESET}"
echo ""
echo -e "${BOLD}Required:${RESET}"

check_required cmake  "CMake"  "https://cmake.org/download/"
check_required ninja  "Ninja"  "https://ninja-build.org/  |  apt install ninja-build  |  brew install ninja"
check_required git    "Git"    "https://git-scm.com/"

# C++ compiler: accept clang++ or g++
if command -v clang++ &>/dev/null; then
    echo -e "  ${GREEN}[OK]${RESET} C++ compiler: $(clang++ --version 2>&1 | head -1)"
elif command -v g++ &>/dev/null; then
    echo -e "  ${GREEN}[OK]${RESET} C++ compiler: $(g++ --version 2>&1 | head -1)"
else
    echo -e "  ${RED}[!!]${RESET} C++ compiler: NOT FOUND"
    echo -e "       ${YELLOW}-> Install g++ (apt install g++ / brew install llvm) or clang++${RESET}"
    MISSING=1
fi

# =============================================================================
# Optional tools
# =============================================================================

echo ""
echo -e "${BOLD}Optional:${RESET}"

check_optional pre-commit   "pre-commit"   "pip install pre-commit  OR  pipx install pre-commit"
check_optional clang-tidy   "clang-tidy"   "apt install clang-tidy / brew install llvm"
check_optional clang-format "clang-format" "apt install clang-format / brew install llvm"

# =============================================================================
# Result
# =============================================================================

echo ""
if [[ $MISSING -eq 0 ]]; then
    echo -e "${GREEN}${BOLD}All required dependencies found.${RESET}"
    exit 0
else
    echo -e "${RED}${BOLD}Some required dependencies are missing. Install them and re-run.${RESET}"
    exit 1
fi
