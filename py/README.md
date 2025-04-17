# Python Setup Guide

This guide explains how to use the new Python-based setup process for the pyalgo project.

## Prerequisites

- Python 3.6 or higher
- Git
- CMake

## Usage

The `setup.py` script replaces the previous npm/yarn-based workflow. Make sure the script is executable:

```bash
chmod +x setup.py
```

### Available Commands

The script provides several commands:

1. **Install dependencies** (replaces `install.sh`):
   ```bash
   ./setup.py install
   ```
   This will clone the pybind11 repository if it doesn't exist.

2. **Setup virtual environment** (replaces `yarn setup`):
   ```bash
   ./setup.py setup
   ```
   This will create a Python virtual environment. After running this command, you'll need to activate the environment separately:
   
   - On Linux/macOS:
     ```bash
     source project_env/bin/activate
     ```
   - On Windows:
     ```bash
     project_env\Scripts\activate
     ```

3. **Build the library** (replaces `yarn build`):
   ```bash
   ./setup.py build
   ```
   This will create the build directory, run CMake, build pyalgo, and move the resulting .so file to the bin directory.

4. **Do everything at once**:
   ```bash
   ./setup.py all
   ```
   This will run all the steps above in sequence.

## Testing the Generated Code

After building, you can test the library:

```bash
cd ../bin
python3 test.py
```

## Packaging

You can create and install the wheel package all at once with:

```bash
./setup.py package
```

This will:
1. Create a wheel package (.whl file)
2. Install the wheel package into your current Python environment

### Testing the installation
After installing the package, test it with:
```bash
python3 ../bin/test.py
```

### Displaying package info
```bash
pip3 show pyalgo
```

### Manual packaging (alternative)
If you need more control, you can still perform the packaging steps manually:

#### Creating the wheel only
```bash
pip3 wheel .
```

#### Installing a specific wheel
```bash
pip3 install YOUR-WHEEL-FILENAME.whl --force-reinstall
```