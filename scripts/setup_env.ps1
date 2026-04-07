# Backend environment setup script (Windows)
# Creates a virtual environment and installs dependencies

$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "========================================="
Write-Host "   Python Environment Setup (Windows)"
Write-Host "========================================="
Write-Host ""

# Resolve project root (one level up from script)
$ProjectRoot = Resolve-Path "$PSScriptRoot/.."
Set-Location $ProjectRoot

Write-Host "Project root: $ProjectRoot"

# Check Python availability
if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Error "Python is not installed or not in PATH."
    exit 1
}

# Virtual environment path
$VenvPath = Join-Path $ProjectRoot ".venv"

# Create virtual environment if missing
if (-not (Test-Path $VenvPath)) {
    Write-Host "Creating virtual environment..."
    python -m venv $VenvPath
} else {
    Write-Host "Virtual environment already exists"
}

# Activate virtual environment
Write-Host "Activating virtual environment..."
& "$VenvPath\Scripts\Activate.ps1"

# Upgrade pip and install dependencies
Write-Host "Installing dependencies..."
python -m pip install --upgrade pip pre-commit

Write-Host ""
Write-Host "Setup completed successfully!"
Write-Host ""
Write-Host "Next steps:"
Write-Host "  Activate: .\.venv\Scripts\Activate.ps1"
Write-Host ""
