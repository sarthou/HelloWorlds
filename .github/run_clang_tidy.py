#!/usr/bin/env python

# Run clang-tidy recursively and parallel on directory
# Usage: run-clang-tidy sourcedir builddir excludedirs extensions
# excludedirs and extensions are specified as comma-separated 
# string without dot, e.g. 'External,test' and 'c,cpp' 

import os
import sys
import subprocess
import multiprocessing
import json
from pathlib import Path

manager = multiprocessing.Manager()
failedfiles = manager.list()

# --- 1. Argument Parsing and Path Setup ---

# Get absolute paths from arguments
if len(sys.argv) < 3:
    print("Usage: python3 run-clang-tidy <sourcedir> <builddir> [excludedirs] [extensions]", file=sys.stderr)
    sys.exit(1)

sourcedir = Path(os.path.abspath(sys.argv[1]))
builddir = Path(os.path.abspath(sys.argv[2]))
print(f"Source directory: {sourcedir}")
print(f"Build directory: {builddir}")

# If exclude dirs is provided, split it and convert to absolute paths
excludedirs = set()
if len(sys.argv) > 3 and sys.argv[3]:
    # Exclude directories should be relative paths (e.g., "External")
    for s in sys.argv[3].split(','):
        # We store the *relative* path component for substring matching
        excludedirs.add(s.strip())
        
print(f"Exclude directories (relative path components): {excludedirs}")

# Split extensions into a tuple for the fallback
extensions = ()
if len(sys.argv) >= 5 and sys.argv[4]:
    extensions = tuple([("." + s) for s in sys.argv[4].split(',')])
print(f"Extensions (for fallback): {extensions}")


# --- 2. Helper Functions ---

def should_exclude(filepath, source_root):
    """Checks if a file should be excluded based on the provided relative exclusion list."""
    try:
        # Get the path relative to the source root
        relative_path = Path(filepath).relative_to(source_root)
    except ValueError:
        # If the file is outside the source directory, it's usually built code, so we exclude it.
        # This handles cases like files within the build directory.
        return True 

    # Check if any part of the relative path contains an exclusion component
    for exclude_component in excludedirs:
        # Check if the exclusion component is in the relative path string
        # This catches "External" inside "External/PhysX/src/file.cpp"
        if exclude_component in str(relative_path):
            return True
            
    return False

def runclangtidy(filepath):
    """Runs clang-tidy on a single file."""
    # Use list form of subprocess.Popen for better security/quoting
    command = ['clang-tidy', '-warnings-as-errors=*', '-p', str(builddir), filepath]
    print(f"Checking: {filepath}")
    
    # We run the command directly without shell=True for better handling of paths/quotes
    proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    stdout, _ = proc.communicate()

    if proc.returncode != 0:
        # Append the file and output to failedfiles
        failedfiles.append((filepath, stdout))
        
def collectcompiledfiles(source_root, build_path):
    """Collects files from compile_commands.json, applying exclusion logic."""
    collectedfiles = []
    compile_commands_path = build_path / 'compile_commands.json'

    if not compile_commands_path.exists():
        print(f"Error: Compilation database not found at {compile_commands_path}", file=sys.stderr)
        return collectedfiles

    try:
        with open(compile_commands_path, 'r') as f:
            compile_data = json.load(f)
    except json.JSONDecodeError:
        print(f"Error: Failed to decode JSON from {compile_commands_path}", file=sys.stderr)
        return collectedfiles

    for line in compile_data:
        # The 'file' field contains the absolute path to the source file
        filepath = Path(line["file"])
        
        # 1. Must be a file within the source directory
        if not filepath.is_relative_to(source_root):
            continue

        # 2. Apply exclusion filter
        if should_exclude(filepath, source_root):
            print(f"Excluding (User rule): {filepath}")
            continue

        collectedfiles.append(str(filepath))

    return collectedfiles

# --- 3. Main Execution ---

# Note: The original fallback logic for collectfiles is removed as the primary goal
# is to use the compilation database, which is generated during a CMake/Colcon build.

pool = multiprocessing.Pool()
files = collectcompiledfiles(sourcedir, builddir)

if len(files) == 0:
    print("Warning: No files found in compile_commands.json that match source directory after exclusions.", file=sys.stderr)
    # Optional: Re-implement collectfiles for a basic fallback, but generally the compile_commands.json should be used.

print(f"Found {len(files)} files to check...")
pool.map(runclangtidy, files)
pool.close()
pool.join()

# --- 4. Final Result ---

if len(failedfiles) > 0:
    print("\n--- ERRORS FOUND ---", file=sys.stderr)
    for filepath, output in failedfiles:
        print(f"\nERROR in file: {filepath}", file=sys.stderr)
        print(output, file=sys.stderr)
    print(f"\nErrors in {len(failedfiles)} files", file=sys.stderr)
    sys.exit(1)
    
print("No errors found")
sys.exit(0)