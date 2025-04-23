#!/usr/bin/env python3
import os
import subprocess
import argparse
import shutil
import sys
from pathlib import Path
import platform
import glob

def run_command(cmd, cwd=None):
    """Run a shell command and print output"""
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd, check=True, text=True, capture_output=True)
    if result.stdout:
        print(result.stdout)
    return result

def ensure_pybind11():
    """Clone pybind11 repository if it doesn't exist"""
    if not os.path.exists("py/pybind11"):
        print("Cloning pybind11 repository...")
        run_command(["git", "clone", "https://github.com/pybind/pybind11.git", "py/pybind11"])
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

def build_cpp_library():
    """Build the C++ library"""
    print("Building C++ library...")
    
    # Create build directory if it doesn't exist
    cpp_build_dir = Path("../cpp/build")
    cpp_build_dir.mkdir(exist_ok=True)
    
    # Run CMake for C++ library
    run_command(["cmake", ".."], cwd=cpp_build_dir)
    run_command(["make"], cwd=cpp_build_dir)
    
    # Check if library was built
    if platform.system() == "Darwin":
        lib_name = "libalgo.dylib"
    elif platform.system() == "Windows":
        lib_name = "algo.dll"
    else:  # Linux
        lib_name = "libalgo.so"
    
    lib_path = Path("bin") / lib_name
    if not lib_path.exists():
        print(f"Warning: {lib_name} not found in bin directory!")
    else:
        print(f"C++ library built successfully: {lib_path}")
    
    return lib_path.exists()

def build_python_binding():
    """Build the pyalgo Python binding using CMake"""
    print("Building Python binding...")
    
    # Create build directory if it doesn't exist
    py_build_dir = Path("build")
    py_build_dir.mkdir(exist_ok=True)
    
    # Run CMake
    run_command(["cmake", ".."], cwd=py_build_dir)
    
    # Build pyalgo
    run_command(["make", "pyalgo"], cwd=py_build_dir)
    
    # Find the generated .so file (platform independent)
    if platform.system() == "Windows":
        so_files = list(py_build_dir.glob("*.pyd"))
        target_ext = ".pyd"
    else:
        so_files = list(py_build_dir.glob("*.so"))
        target_ext = ".so"
    
    if so_files:
        source = so_files[0]
        bin_dir = Path("bin")
        bin_dir.mkdir(exist_ok=True)
        target = bin_dir / f"pyalgo{target_ext}"
        shutil.copy2(source, target)
        print(f"Copied {source} to {target}")
    else:
        print("Could not find generated Python binding file.")
        return False
    
    return True

def prepare_package_data():
    """Prepare data for packaging"""
    print("Preparing package data...")
    
    # Create package directories
    package_dir = Path("build/pyalgo_package")
    package_dir.mkdir(exist_ok=True)
    
    # Create __init__.py
    init_file = package_dir / "__init__.py"
    with open(init_file, "w") as f:
        f.write("""
from .pyalgo import *

__version__ = '0.1.0'
""")
    
    # Copy binary files to package directory
    bin_dir = Path("../bin")
    
    # Copy Python module
    if platform.system() == "Windows":
        py_module = bin_dir / "pyalgo.pyd"
        target_module = package_dir / "pyalgo.pyd"
    else:
        py_module = bin_dir / "pyalgo.so"
        target_module = package_dir / "pyalgo.so"
    
    if py_module.exists():
        shutil.copy2(py_module, target_module)
        print(f"Copied {py_module} to {target_module}")
    else:
        print(f"Error: Python module {py_module} not found!")
        return False
    
    # Copy C++ library
    if platform.system() == "Darwin":
        lib_name = "libalgo.dylib"
    elif platform.system() == "Windows":
        lib_name = "algo.dll"
    else:  # Linux
        lib_name = "libalgo.so"
    
    cpp_lib = bin_dir / lib_name
    target_lib = package_dir / lib_name
    
    if cpp_lib.exists():
        shutil.copy2(cpp_lib, target_lib)
        print(f"Copied {cpp_lib} to {target_lib}")
    else:
        print(f"Error: C++ library {cpp_lib} not found!")
        return False
    
    # Create setup.py for packaging
    setup_file = Path("../setup.py")
    with open(setup_file, "w") as f:
        f.write(f"""
from setuptools import setup, find_packages
import platform

# Get library name based on platform
if platform.system() == "Darwin":
    lib_name = "libalgo.dylib"
elif platform.system() == "Windows":
    lib_name = "algo.dll"
else:  # Linux
    lib_name = "libalgo.so"

print("-----> ", "pyalgo.*", "../bin/"+lib_name)

setup(
    name="pyalgo",
    version="0.1.0",
    author="Frantz Maerten",
    author_email="fmaerten@gmail.com",
    description="C++ binding in Python for stress field computation",
    packages=find_packages(),
    package_data={{
        "pyalgo_package": ["pyalgo.*", "../bin/"+lib_name],
    }},
    python_requires=">=3.6",
)
""")
    
    return True

def create_wheel():
    """Create a wheel package"""
    print("Creating wheel package...")
    # run_command([sys.executable, "-m", "pip", "install", "wheel", "setuptools"])
    run_command([sys.executable, "-m", "pip", "wheel", "build/"], cwd=".")
    
    # Find the created wheel
    wheels = list(Path(".").glob("pyalgo-*.whl"))
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
    if prepare_package_data():
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
build    - Build the C++ library and Python binding
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
    build_parser = subparsers.add_parser("build", help="Build C++ library and Python binding")
    
    # Package command
    package_parser = subparsers.add_parser("package", help="Create and install wheel package")
    
    # All command
    all_parser = subparsers.add_parser("all", help="Run install, setup and build")
    
    args = parser.parse_args()
    
    if args.command == "help":
        show_help()
        return
    
    if args.command == "install":
        ensure_pybind11()
        return
    
    if args.command == "setup":
        setup_venv()
        return
    
    if args.command == "build":
        build_cpp_library()
        build_python_binding()
        return
    
    if args.command == "package":
        package()
        return
    
    if args.command == "all":
        ensure_pybind11()
        setup_venv()
        build_cpp_library()
        build_python_binding()
        package()
        return
    
    # If no command or invalid command
    parser.print_help()
    print("\nFor more detailed information, run: ./setup.py help")

if __name__ == "__main__":
    main()