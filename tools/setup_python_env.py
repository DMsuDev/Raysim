#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
VENV_DIR = ROOT / ".venv"
REQUIREMENTS = ROOT / "requirements.txt"

def run(cmd):
    subprocess.run(cmd, check=True)

def main():
    print("Creating Python virtual environment...")
    if not VENV_DIR.exists():
        run([sys.executable, "-m", "venv", str(VENV_DIR)])

    pip = VENV_DIR / "bin" / "pip"
    if not pip.exists():  # Windows fallback
        pip = VENV_DIR / "Scripts" / "pip.exe"

    print("Installing Python dependencies...")
    if REQUIREMENTS.exists():
        run([str(pip), "install", "-r", str(REQUIREMENTS)])

    print("Installing pre-commit...")
    run([str(pip), "install", "pre-commit"])

    print("Python environment ready!")

if __name__ == "__main__":
    main()
