$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$Vcpkg = Join-Path $Root "vcpkg"

Write-Host "Bootstrapping vcpkg..."
& "$Vcpkg\bootstrap-vcpkg.bat"

Write-Host "Installing dependencies from vcpkg.json..."
& "$Vcpkg\vcpkg.exe" install --clean-after-build

Write-Host "vcpkg initialized!"
