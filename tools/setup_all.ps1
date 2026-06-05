# =============================================================================
# Raysim - setup_all.ps1
# Bootstraps the development environment.
#
# Steps performed:
#   1. Initialise git submodules (vcpkg)
#   2. Bootstrap vcpkg and install dependencies
#   3. Install pre-commit hooks (optional)
#
# Supports Windows, Linux, and macOS via PowerShell Core (pwsh 7+).
# Legacy Windows PowerShell (5.x) is also supported but Windows-only.
#
# Usage: .\tools\setup_all.ps1 [-SkipVcpkg] [-SkipPreCommit]
# =============================================================================
[CmdletBinding()]
param(
    [switch]$SkipVcpkg,
    [switch]$SkipPreCommit
)

$ErrorActionPreference = "Stop"

# =============================================================================
# Helpers
# =============================================================================

function Write-Info    { param($msg) Write-Host "[setup] $msg" -ForegroundColor Cyan    }
function Write-Success { param($msg) Write-Host "[setup] $msg" -ForegroundColor Green   }
function Write-Warn    { param($msg) Write-Host "[setup] $msg" -ForegroundColor Yellow  }
function Write-Fail    { param($msg) Write-Error "[setup] ERROR: $msg"                  }

# =============================================================================
# Environment detection
# =============================================================================

# $IsWindows / $IsLinux / $IsMacOS are defined in PowerShell Core (pwsh 6+).
# Assume Windows when running legacy Windows PowerShell (Major < 6).
$OnWindows = $IsWindows -or ($PSVersionTable.PSVersion.Major -lt 6)
$RepoRoot  = Split-Path -Parent $PSScriptRoot
Set-Location $RepoRoot

Write-Host "`n=== Raysim Setup ===" -ForegroundColor White
Write-Host ""

# =============================================================================
# Git submodules
# =============================================================================

$VcpkgMarker = if ($OnWindows) { "vcpkg\bootstrap-vcpkg.bat" } else { "vcpkg/bootstrap-vcpkg.sh" }
if (-not (Test-Path $VcpkgMarker)) {
    Write-Info "Initialising git submodules..."
    git submodule update --init --recursive
}

# =============================================================================
# vcpkg bootstrap and dependency install
# =============================================================================

if (-not $SkipVcpkg) {
    Write-Info "Bootstrapping vcpkg..."
    if ($OnWindows) {
        & "vcpkg\bootstrap-vcpkg.bat" -disableMetrics
    } else {
        bash vcpkg/bootstrap-vcpkg.sh -disableMetrics
    }
    if ($LASTEXITCODE -ne 0) { Write-Fail "vcpkg bootstrap failed." }

    # Resolve the vcpkg triplet for the current platform and architecture
    $Arch = if ([System.Environment]::Is64BitOperatingSystem) { "x64" } else { "x86" }
    if     ($IsLinux)  { $Triplet = "$Arch-linux"   }
    elseif ($IsMacOS)  { $Triplet = "$Arch-osx"     }
    else               { $Triplet = "$Arch-windows"  }

    Write-Info "Installing vcpkg dependencies (triplet: $Triplet)..."
    $VcpkgExe = if ($OnWindows) { "vcpkg\vcpkg.exe" } else { "./vcpkg/vcpkg" }
    & $VcpkgExe install --triplet $Triplet
    if ($LASTEXITCODE -ne 0) { Write-Fail "vcpkg install failed." }
    Write-Success "vcpkg ready."
} else {
    Write-Warn "Skipping vcpkg (-SkipVcpkg)."
}

# =============================================================================
# pre-commit hooks
# =============================================================================

if (-not $SkipPreCommit) {
    $pcCmd = Get-Command pre-commit -ErrorAction SilentlyContinue
    if (-not $pcCmd) {
        Write-Warn "pre-commit not found. Attempting global install..."
        if (Get-Command pipx -ErrorAction SilentlyContinue) {
            pipx install pre-commit
        } elseif (Get-Command pip3 -ErrorAction SilentlyContinue) {
            pip3 install --user pre-commit
        } elseif (Get-Command pip -ErrorAction SilentlyContinue) {
            pip install --user pre-commit
        } else {
            Write-Fail "pip/pipx not found. Install pre-commit manually:`n  pip install pre-commit  OR  pipx install pre-commit"
        }
    }
    Write-Info "Installing pre-commit hooks..."
    pre-commit install
    if ($LASTEXITCODE -ne 0) { Write-Fail "pre-commit install failed." }
    Write-Success "pre-commit hooks installed."
} else {
    Write-Warn "Skipping pre-commit (-SkipPreCommit)."
}

# =============================================================================
# Done
# =============================================================================

Write-Host ""
Write-Success "Setup complete!"
Write-Host "Next steps:"
Write-Host "  cmake --preset debug          # configure"
Write-Host "  cmake --build --preset debug  # build"
Write-Host ""
