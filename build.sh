#!/bin/bash

# Exit on error
set -e

# Create build directory if it doesn't exist
mkdir -p build

# Enter build directory
cd build

# Clean previous build if it exists
if [ -f "CMakeCache.txt" ]; then
    echo "Cleaning previous build..."
    rm -rf *
fi

# Configure CMake
echo "Configuring CMake..."
cmake .. || {
    echo "CMake configuration failed"
    exit 1
}

# Build the project
echo "Building project..."
make -j$(nproc) || {
    echo "Build failed"
    exit 1
}

echo "Build successful!"
echo "Executable is located in: $(pwd)/bin/"
echo "A binary named "ctrace-cli" is is needed at the same directory as the executable to run the program."

# Return to the original directory
cd ..
