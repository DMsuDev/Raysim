# Bootstrap vcpkg and install project dependencies.
# Windows (PowerShell). Called automatically by setup_all.ps1.
# Stops on first error.

$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$Vcpkg = Join-Path $Root "vcpkg"

Write-Host "Bootstrapping vcpkg..."
try {
    & "$Vcpkg\bootstrap-vcpkg.bat"
} catch {
    Write-Host "vcpkg bootstrap failed. Check the output above for details."
    exit 1
}

Write-Host "Installing dependencies from vcpkg.json..."
try {
    & "$Vcpkg\vcpkg.exe" install --clean-after-build
} catch {
    Write-Host "vcpkg install failed. Check the output above for details."
    exit 1
}

Write-Host "vcpkg ready."
