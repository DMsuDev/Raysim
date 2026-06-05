# =============================================================================
# Raysim - check_deps.ps1
# Verifies that all required and optional build tools are available on PATH.
# Exits 0 if all REQUIRED tools are found, 1 if any are missing.
# Optional tools report their status but do not affect the exit code.
# =============================================================================
$MissingRequired = $false

# =============================================================================
# Helpers
# =============================================================================

function Check-Required {
    param(
        [string]$Command,
        [string]$Label = $Command,
        [string]$Hint  = ""
    )
    $found = Get-Command $Command -ErrorAction SilentlyContinue
    if ($found) {
        try   { $ver = (& $Command --version 2>&1 | Select-Object -First 1) }
        catch { $ver = "(unknown version)" }
        Write-Host "  [OK] $Label`: $ver" -ForegroundColor Green
    } else {
        Write-Host "  [!!] $Label`: NOT FOUND" -ForegroundColor Red
        if ($Hint) { Write-Host "       -> $Hint" -ForegroundColor Yellow }
        $script:MissingRequired = $true
    }
}

function Check-Optional {
    param(
        [string]$Command,
        [string]$Label = $Command,
        [string]$Hint  = ""
    )
    $found = Get-Command $Command -ErrorAction SilentlyContinue
    if ($found) {
        try   { $ver = (& $Command --version 2>&1 | Select-Object -First 1) }
        catch { $ver = "(unknown version)" }
        Write-Host "  [ok] $Label`: $ver" -ForegroundColor Cyan
    } else {
        Write-Host "  [--] $Label`: not found" -ForegroundColor DarkGray
        if ($Hint) { Write-Host "       -> $Hint" -ForegroundColor DarkGray }
    }
}

# =============================================================================
# Required tools
# =============================================================================

Write-Host "`n=== Raysim Dependency Check ===" -ForegroundColor White
Write-Host ""
Write-Host "Required:" -ForegroundColor White

Check-Required cmake  "CMake"  "https://cmake.org/download/"
Check-Required ninja  "Ninja"  "winget install Ninja-build.Ninja  OR  choco install ninja"
Check-Required git    "Git"    "https://git-scm.com/"

# Compiler: accept clang-cl (LLVM/MSVC frontend), cl (MSVC), clang++, or g++ (MinGW/GCC)
$compilerFound = $false
foreach ($comp in @("clang-cl", "cl", "clang++", "g++")) {
    if (Get-Command $comp -ErrorAction SilentlyContinue) {
        try { $ver = (& $comp --version 2>&1 | Select-Object -First 1) } catch { $ver = "" }
        Write-Host "  [OK] C++ compiler ($comp): $ver" -ForegroundColor Green
        $compilerFound = $true
        break
    }
}
if (-not $compilerFound) {
    Write-Host "  [!!] C++ compiler: NOT FOUND" -ForegroundColor Red
    Write-Host "       -> Install Visual Studio Build Tools, LLVM, or MinGW" -ForegroundColor Yellow
    $MissingRequired = $true
}

# =============================================================================
# Optional tools
# =============================================================================

Write-Host ""
Write-Host "Optional:" -ForegroundColor White

Check-Optional pre-commit   "pre-commit"   "pip install pre-commit"
Check-Optional clang-tidy   "clang-tidy"   "Install LLVM: https://releases.llvm.org/"
Check-Optional clang-format "clang-format" "Install LLVM: https://releases.llvm.org/"

# =============================================================================
# Result
# =============================================================================

Write-Host ""
if (-not $MissingRequired) {
    Write-Host "All required dependencies found." -ForegroundColor Green
    exit 0
} else {
    Write-Host "Some required dependencies are missing. Install them and re-run." -ForegroundColor Red
    exit 1
}
