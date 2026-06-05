# =============================================================================
# Raysim - run_example.ps1
# Finds and launches a compiled example binary by name.
#
# Binary locations (from CMakePresets.json binaryDir + RS output structure):
#   Ninja presets  -> build/ninja/<preset>/bin/<Name>[.exe]
#   MSVC presets   -> build/msvc/<Debug|Release>/bin/<Name>[.exe]
#
# Usage: .\tools\run_example.ps1 -Name <ExampleName> [-Preset <preset>]
# Example:
#   .\tools\run_example.ps1 -Name BouncingBalls -Preset release
# =============================================================================
[CmdletBinding()]
param(
    [Parameter(Mandatory = $false)]
    [string]$Name = "",
    [string]$Preset = "debug"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot

# =============================================================================
# Show available examples when no name is provided
# =============================================================================

if (-not $Name) {
    Write-Host "Usage: .\tools\run_example.ps1 -Name <ExampleName> [-Preset <preset>]"
    Write-Host ""
    Write-Host "Available examples:"
    $exDir = Join-Path $RepoRoot "examples"
    if (Test-Path $exDir) {
        Get-ChildItem $exDir -Directory | ForEach-Object { Write-Host "  $($_.Name)" }
    }
    exit 1
}

# =============================================================================
# Preset -> build directory mapping
# =============================================================================

function Get-PresetBuildDir {
    param([string]$P)
    if ($P -match "msvc")             { return Join-Path $RepoRoot "build\msvc" }
    if ($P -match "relwithdebinfo")   { return Join-Path $RepoRoot "build\ninja\relwithdebinfo" }
    if ($P -match "release")          { return Join-Path $RepoRoot "build\ninja\release" }
    if ($P -match "debug|sanitized")  { return Join-Path $RepoRoot "build\ninja\debug" }
    return Join-Path $RepoRoot "build\$P"
}

# =============================================================================
# Locate binary
# =============================================================================

# PowerShell Core on Linux/macOS: $IsWindows is $false.
# Legacy Windows PowerShell (5.x): $IsWindows is not defined but always Windows.
$OnWindows = $IsWindows -or ($PSVersionTable.PSVersion.Major -lt 6)
$BinFile   = if ($OnWindows) { "$Name.exe" } else { $Name }

$BuildDir = Get-PresetBuildDir $Preset
if (-not (Test-Path $BuildDir)) {
    Write-Error "Build directory not found: $BuildDir`n  Run: .\tools\build.ps1 -Preset $Preset"
}

$Binary = Get-ChildItem -Path $BuildDir -Recurse -Filter $BinFile -ErrorAction SilentlyContinue |
          Select-Object -First 1

if (-not $Binary) {
    Write-Error "Binary '$BinFile' not found under $BuildDir.`n  Build first: .\tools\build.ps1 -Preset $Preset"
}

# =============================================================================
# Launch
# =============================================================================

Write-Host "[run] $($Binary.FullName)" -ForegroundColor Cyan
& $Binary.FullName
