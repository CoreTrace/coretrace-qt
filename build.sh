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
mv coretrace-qt ../coretrace-qt
echo "Build successful!"
echo "Executable is located in: $(pwd)/bin/"
echo "A binary named "ctrace-cli" is is needed at the same directory as the executable to run the program."

# Return to the original directory
cd ..
echo "Cloning into coretrace and building it..."
sleep 3
git clone https://github.com/CoreTrace/coretrace.git
cd coretrace
# Edit AnalysisTools.hpp: comment out original line 154 and insert new code
file="include/Process/Tools/AnalysisTools.hpp"
line_num=154
tmpfile=$(mktemp)

awk -v n="$line_num" '
NR==n {
    print "// " $0
    print "        argsProcess.push_back(\"coretrace/build/flawfinder/src/flawfinder-build/flawfinder.py\");"
    next
}
{ print }
' "$file" > "$tmpfile" && mv "$tmpfile" "$file"

mkdir -p build && cd build
cmake ..                        \
    -DPARSER_TYPE=CLI11         \
    -DUSE_ADDRESS_SANITIZER=OFF

make -j$(nproc)
cd ../../
./coretrace-qt