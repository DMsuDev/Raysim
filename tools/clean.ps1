# =============================================================================
# Raysim - clean.ps1
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
# Usage: .\tools\clean.ps1 [-Target debug|release|relwithdebinfo|msvc|ninja|all]
# =============================================================================
[CmdletBinding()]
param(
    [ValidateSet("debug","release","relwithdebinfo","msvc","ninja","all")]
    [string]$Target = "all"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $RepoRoot

# =============================================================================
# Helpers
# =============================================================================

function Remove-Dir {
    param([string]$Path)
    if (Test-Path $Path) {
        Write-Host "[clean] Removing $Path..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $Path
    } else {
        Write-Host "[clean] $Path does not exist, skipping." -ForegroundColor DarkGray
    }
}

# =============================================================================
# Clean
# =============================================================================

switch ($Target) {
    "debug"          { Remove-Dir "build\ninja\debug" }
    "release"        { Remove-Dir "build\ninja\release" }
    "relwithdebinfo" { Remove-Dir "build\ninja\relwithdebinfo" }
    "msvc"           { Remove-Dir "build\msvc" }
    "ninja"          { Remove-Dir "build\ninja" }
    "all" {
        Remove-Dir "build\ninja"
        Remove-Dir "build\msvc"
        if (Test-Path "vcpkg\buildtrees") {
            Write-Host "[clean] Removing vcpkg\buildtrees..." -ForegroundColor Yellow
            Remove-Item -Recurse -Force "vcpkg\buildtrees"
        }
    }
}

Write-Host "[clean] Done." -ForegroundColor Green
