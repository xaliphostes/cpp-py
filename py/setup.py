#!/usr/bin/env python3
import os
import subprocess
import argparse
import shutil
import sys
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run a shell command and print output"""
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd, check=True, text=True, capture_output=True)
    if result.stdout:
        print(result.stdout)
    return result

def ensure_pybind11():
    """Clone pybind11 repository if it doesn't exist"""
    if not os.path.exists("pybind11"):
        print("Cloning pybind11 repository...")
        run_command(["git", "clone", "https://github.com/pybind/pybind11.git"])
    else:
        print("pybind11 repository already exists.")

def setup_venv():
    """Create and activate virtual environment"""
    print("Setting up virtual environment...")
    if not os.path.exists("project_env"):
        run_command([sys.executable, "-m", "pip", "install", "virtualenv"])
        run_command([sys.executable, "-m", "virtualenv", "project_env"])
    
    # Print activation instructions (can't activate from within script)
    venv_path = os.path.abspath("project_env")
    if os.name == 'nt':  # Windows
        print(f"\nTo activate the virtual environment, run:")
        print(f"{venv_path}\\Scripts\\activate")
    else:  # Unix-like
        print(f"\nTo activate the virtual environment, run:")
        print(f"source {venv_path}/bin/activate")

def build_library():
    """Build the pyalgo library using CMake"""
    print("Building pyalgo library...")
    
    # Create build directory if it doesn't exist
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)
    
    # Run CMake
    run_command(["cmake", ".."], cwd=build_dir)
    
    # Build pyalgo
    run_command(["make", "pyalgo"], cwd=build_dir)
    
    # Move the .so file to bin directory
    bin_dir = Path("../../bin")
    bin_dir.mkdir(exist_ok=True)
    
    # Find the generated .so file (platform independent)
    so_files = list(build_dir.glob("*.so"))
    if not so_files:
        so_files = list(build_dir.glob("*.pyd"))  # Windows
    
    if so_files:
        source = so_files[0]
        target = bin_dir / "pyalgo.so"
        shutil.copy2(source, target)
        print(f"Copied {source} to {target}")
    else:
        print("Could not find generated library file.")
        return False
    
    return True

def create_wheel():
    """Create a wheel package"""
    print("Creating wheel package...")
    run_command([sys.executable, "-m", "pip", "wheel", "."])
    
    # Find the created wheel
    wheels = list(Path(".").glob("*.whl"))
    if not wheels:
        print("No wheel was created!")
        return None
    
    print(f"Created wheel: {wheels[0]}")
    return wheels[0]

def install_wheel(wheel_path):
    """Install the created wheel"""
    if not wheel_path or not os.path.exists(wheel_path):
        print("No wheel file found to install!")
        return False
    
    print(f"Installing wheel: {wheel_path}")
    run_command([sys.executable, "-m", "pip", "install", str(wheel_path), "--force-reinstall"])
    print("Wheel installed successfully!")
    return True

def package():
    """Create and install wheel"""
    wheel_path = create_wheel()
    if wheel_path:
        install_wheel(wheel_path)
        return True
    return False

def show_help():
    """Display detailed help information about using the script"""
    help_text = """
PYALGO SETUP UTILITY
====================

This script helps you set up, build, and package the pyalgo library.

AVAILABLE COMMANDS:
------------------

help     - Show this detailed help message
install  - Clone pybind11 repository if it doesn't exist
setup    - Create a Python virtual environment
build    - Build the pyalgo library using CMake
package  - Create and install a wheel package
all      - Run install, setup, and build in sequence

TYPICAL WORKFLOW:
---------------

First time setup:
  ./setup.py all        # Setup everything
  source project_env/bin/activate  # Activate the virtual environment

After making changes:
  ./setup.py build      # Rebuild the library
  
For distribution:
  ./setup.py package    # Create and install wheel package

EXAMPLES:
--------

# First-time setup
./setup.py all

# Just build the library after code changes
./setup.py build

# Create and install wheel package
./setup.py package

For more information, see the README.md file.
"""
    print(help_text)

def main():
    parser = argparse.ArgumentParser(description="Setup and build pyalgo")
    subparsers = parser.add_subparsers(dest="command", help="Command to run")
    
    # Help command
    help_parser = subparsers.add_parser("help", help="Show detailed help information")
    
    # Install command
    install_parser = subparsers.add_parser("install", help="Install dependencies")
    
    # Setup command
    setup_parser = subparsers.add_parser("setup", help="Setup virtual environment")
    
    # Build command
    build_parser = subparsers.add_parser("build", help="Build pyalgo library")
    
    # Package command
    package_parser = subparsers.add_parser("package", help="Create and install wheel package")
    
    # All command
    all_parser = subparsers.add_parser("all", help="Run install, setup and build")
    
    args = parser.parse_args()
    
    if args.command == "help":
        show_help()
        exit()

    if args.command == "install" or args.command == "all":
        ensure_pybind11()
        setup_venv()
        build_library()
        package()
        exit()

    if args.command == "install" or args.command == "all":
        ensure_pybind11()
        exit()
    
    if args.command == "setup" or args.command == "all":
        setup_venv()
        exit()
    
    if args.command == "build" or args.command == "all":
        build_library()
        exit()
    
    if args.command == "package":
        package()
        exit()
    
    parser.print_help()
    print("\nFor more detailed information, run: ./setup.py help")

if __name__ == "__main__":
    main()