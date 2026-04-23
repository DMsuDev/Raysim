#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
VENV = ROOT / ".venv"

def run(cmd):
    subprocess.run(cmd, check=True)

def main():
    print("Ensuring pre-commit is installed...")

    pip = VENV / "bin" / "pip"
    precommit = VENV / "bin" / "pre-commit"

    if not pip.exists():  # Windows fallback
        pip = VENV / "Scripts" / "pip.exe"
        precommit = VENV / "Scripts" / "pre-commit.exe"

    run([str(pip), "install", "pre-commit"])

    print("Installing git hooks...")
    run([str(precommit), "install"])

    print("Pre-commit hooks installed!")

if __name__ == "__main__":
    main()
