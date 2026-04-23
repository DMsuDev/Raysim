$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$Tools = Join-Path $Root "tools"

Write-Host "Setting up Python environment..."
python "$Tools/setup_python_env.py"

Write-Host "Installing pre-commit hooks..."
python "$Tools/install_precommit.py"

Write-Host "Initializing vcpkg..."
& "$Tools/init_vcpkg.ps1"

Write-Host "All tools installed successfully!"
