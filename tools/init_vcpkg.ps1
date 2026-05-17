# Bootstrap vcpkg and install project dependencies.
# Windows (PowerShell). Called automatically by setup_all.ps1.
# Stops on first error.

$ErrorActionPreference = "Stop"

# Resolve script and project root
$ScriptPath = $MyInvocation.MyCommand.Path
$ScriptDir  = Split-Path -Parent $ScriptPath
$Root       = Split-Path -Parent $ScriptDir
$VcpkgDir   = Join-Path $Root "vcpkg"

function Fail([string]$Message) {
    Write-Host $Message
    exit 1
}

if (-not (Test-Path (Join-Path $VcpkgDir "bootstrap-vcpkg.bat"))) {
    Write-Host "vcpkg not found. Initializing submodules into $VcpkgDir..."
    try {
        git submodule update --init --recursive
    } catch {
        Fail "ERROR: Failed to initialize vcpkg submodule or other submodules. Check the output above for details `nThis operation requires:`n  - An active Internet connection`n  - A git repository already initialized (git init) and with submodules configured`nOtherwise, you can manually clone the vcpkg repository into the 'vcpkg' directory at the project root."
    }
}

$BootstrapBat = Join-Path $VcpkgDir "bootstrap-vcpkg.bat"
$VcpkgExe     = Join-Path $VcpkgDir "vcpkg.exe"

Write-Host "Bootstrapping vcpkg..."
if (Test-Path $BootstrapBat) {
    & $BootstrapBat
    if ($LASTEXITCODE -ne 0) {
        Fail "vcpkg bootstrap failed (bat exit code $LASTEXITCODE). Check the output above for details."
    }
} else {
    Fail "No bootstrap script found in '$VcpkgDir'. Check that vcpkg is available."
}

Write-Host "Installing dependencies from vcpkg.json..."
if (Test-Path $VcpkgExe) {
    & $VcpkgExe install --clean-after-build
    if ($LASTEXITCODE -ne 0) { Fail "vcpkg install failed. Check the output above for details." }
} else {
    $VcpkgCmd = Get-Command vcpkg -ErrorAction SilentlyContinue
    if ($null -ne $VcpkgCmd) {
        & $VcpkgCmd.Path install --clean-after-build
        if ($LASTEXITCODE -ne 0) { Fail "vcpkg install failed. Check the output above for details." }
    } else {
        Fail "vcpkg executable not found at '$VcpkgExe' and no 'vcpkg' in PATH."
    }
}

Write-Host "vcpkg ready."
