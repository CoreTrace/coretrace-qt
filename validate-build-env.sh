#!/bin/bash

# Quick AppImage Build Validation Script
# Performs basic checks before attempting a full build

set -e

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_check() {
    echo -n "Checking $1... "
}

print_ok() {
    echo -e "${GREEN}OK${NC}"
}

print_fail() {
    echo -e "${RED}FAIL${NC}"
}

print_warn() {
    echo -e "${YELLOW}WARN${NC}"
}

echo "CoreTrace Qt - AppImage Build Validation"
echo "========================================"

# Check Docker
print_check "Docker installation"
if command -v docker >/dev/null 2>&1; then
    print_ok
else
    print_fail
    echo "❌ Docker is required for AppImage build"
    exit 1
fi

# Check Docker daemon
print_check "Docker daemon"
if docker info >/dev/null 2>&1; then
    print_ok
else
    print_fail
    echo "❌ Docker daemon is not running"
    exit 1
fi

# Check build scripts
print_check "build scripts"
if [ -f "./app-image-builder.sh" ] && [ -f "./build.sh" ]; then
    print_ok
else
    print_fail
    echo "❌ Required build scripts not found"
    exit 1
fi

# Check CMake configuration
print_check "CMake configuration"
if [ -f "./CMakeLists.txt" ]; then
    print_ok
else
    print_fail
    echo "❌ CMakeLists.txt not found"
    exit 1
fi

# Check source files
print_check "source files"
if [ -d "./src" ] && [ -d "./includes" ]; then
    SRC_COUNT=$(find ./src -name "*.cpp" | wc -l)
    HEADER_COUNT=$(find ./includes -name "*.hpp" | wc -l)
    if [ $SRC_COUNT -gt 0 ] && [ $HEADER_COUNT -gt 0 ]; then
        print_ok
        echo "  Found $SRC_COUNT source files and $HEADER_COUNT headers"
    else
        print_warn
        echo "  ⚠️  Low number of source files ($SRC_COUNT) or headers ($HEADER_COUNT)"
    fi
else
    print_fail
    echo "❌ Source directories not found"
    exit 1
fi

# Check disk space
print_check "disk space"
AVAILABLE_GB=$(df . | awk 'NR==2 {print int($4/1024/1024)}')
if [ $AVAILABLE_GB -gt 5 ]; then
    print_ok
    echo "  Available: ${AVAILABLE_GB}GB"
else
    print_warn
    echo "  ⚠️  Low disk space: ${AVAILABLE_GB}GB (recommended: >5GB)"
fi

# Check memory
print_check "available memory"
AVAILABLE_MEM_GB=$(free -g | awk 'NR==2{print $7}')
if [ $AVAILABLE_MEM_GB -gt 2 ]; then
    print_ok
    echo "  Available: ${AVAILABLE_MEM_GB}GB"
else
    print_warn
    echo "  ⚠️  Low memory: ${AVAILABLE_MEM_GB}GB (recommended: >2GB)"
fi

echo
echo "✅ Validation completed successfully!"
echo
echo "To run the full AppImage build test:"
echo "  ./test-appimage-build.sh"
echo
echo "To build AppImage manually:"
echo "  ./app-image-builder.sh"
echo "  cd appimage-build-full && ./build-in-docker-full.sh"
