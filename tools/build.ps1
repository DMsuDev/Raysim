# =============================================================================
# Raysim - build.ps1
# Thin wrapper around cmake --preset for convenience.
#
# Common presets (CMakePresets.json):
#   debug            Debug + sanitizers    -> build/ninja/debug
#   release          Release + LTO         -> build/ninja/release
#   relwithdebinfo   Release with symbols  -> build/ninja/relwithdebinfo
#   msvc-debug       MSVC Debug            -> build/msvc
#   msvc-release     MSVC Release          -> build/msvc
#
# User presets (CMakeUserPresets.json):
#   local-ninja-debug      local-ninja-release      local-ninja-sanitized
#   local-msvc-debug       local-msvc-release
#
# Usage: .\tools\build.ps1 [-Preset <name>] [-Clean] [-ConfigOnly]
# =============================================================================
[CmdletBinding()]
param(
    [string]$Preset     = "debug",
    [switch]$Clean,
    [switch]$ConfigOnly     # configure only, skip the build step
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $RepoRoot

function Write-Info { param($msg) Write-Host "[build] $msg" -ForegroundColor Cyan }

# =============================================================================
# Preset -> build directory mapping
# =============================================================================
# Used only for -Clean; cmake --preset derives binaryDir itself from the preset JSON.

function Get-PresetBuildDir {
    param([string]$P)
    if ($P -match "msvc")             { return Join-Path $RepoRoot "build\msvc" }
    if ($P -match "relwithdebinfo")   { return Join-Path $RepoRoot "build\ninja\relwithdebinfo" }
    if ($P -match "release")          { return Join-Path $RepoRoot "build\ninja\release" }
    if ($P -match "debug|sanitized")  { return Join-Path $RepoRoot "build\ninja\debug" }
    return Join-Path $RepoRoot "build\$P"
}

# =============================================================================
# Clean (optional)
# =============================================================================

if ($Clean) {
    $BuildDir = Get-PresetBuildDir $Preset
    if (Test-Path $BuildDir) {
        Write-Info "Cleaning $BuildDir..."
        Remove-Item -Recurse -Force $BuildDir
    } else {
        Write-Info "Nothing to clean -- build directory does not exist."
    }
}

# =============================================================================
# Configure
# =============================================================================

Write-Info "Configuring preset: $Preset..."
cmake --preset $Preset
if ($LASTEXITCODE -ne 0) { throw "cmake configure failed (exit $LASTEXITCODE)." }

if ($ConfigOnly) {
    Write-Host "`n[build] Configure-only mode -- skipping build step.`n" -ForegroundColor Green
    exit 0
}

# =============================================================================
# Build
# =============================================================================

Write-Info "Building preset: $Preset..."
cmake --build --preset $Preset
if ($LASTEXITCODE -ne 0) { throw "cmake build failed (exit $LASTEXITCODE)." }

Write-Host "`n[build] Done -- preset: $Preset`n" -ForegroundColor Green
