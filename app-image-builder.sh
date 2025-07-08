#!/bin/bash
# Create a Docker environment to compile and package your application

# Create a directory for building
mkdir -p appimage-build-full
cd appimage-build-full

# Create a Dockerfile for building
cat > Dockerfile << 'EOF'
FROM ubuntu:20.04

# Set up environment
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qt5-default \
    qtbase5-dev \
    qttools5-dev \
    libqt5widgets5 \
    libqt5gui5 \
    libqt5core5a \
    fuse \
    wget \
    file \
    libfuse2 \
    git \
    libxcb-util1 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-icccm4 \
    libxcb-render-util0 \
    libxcb-xinput0 \
    libxcb-xkb1 \
    libxkbcommon-x11-0 \
    libwayland-client0 \
    libwayland-cursor0 \
    libwayland-egl1 \
    libxkbcommon0

# Set up work directory
WORKDIR /app
COPY . /app

# Install linuxdeployqt
RUN wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" && \
    chmod +x linuxdeployqt-continuous-x86_64.AppImage

# Entry point for building
ENTRYPOINT ["/bin/bash"]
EOF

# Create a build script to run inside the container
cat > build-compile-appimage.sh << 'EOF'
#!/bin/bash
set -e

# Copy source code
mkdir -p /app/src
cp -r /src/* /app/src/

# Build the application
cd /app/src
mkdir -p build
cd build
cmake ..
make -j$(nproc)

# Create AppDir
mkdir -p /app/CTraceQt.AppDir/usr/bin/
cp /app/src/build/coretrace-qt /app/CTraceQt.AppDir/usr/bin/

# Create desktop file
mkdir -p /app/CTraceQt.AppDir/usr/share/applications/
cat > /app/CTraceQt.AppDir/usr/share/applications/coretrace-qt.desktop << EOD
[Desktop Entry]
Name=CoreTrace Qt
Exec=coretrace-qt
Icon=coretrace
Type=Application
Categories=Development;
EOD

# Create AppRun script
cat > /app/CTraceQt.AppDir/AppRun << EOD
#!/bin/bash
HERE="\$(dirname "\$(readlink -f "\${0}")")"
export PATH="\${HERE}/usr/bin:\${PATH}"
export LD_LIBRARY_PATH="\${HERE}/usr/lib:\${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="\${HERE}/usr/lib/qt5/plugins"
export QML2_IMPORT_PATH="\${HERE}/usr/lib/qml"
export QT_QPA_PLATFORM_PLUGIN_PATH="\${HERE}/usr/lib/qt5/plugins/platforms"

# Try to autodetect the platform
if [ -n "\$WAYLAND_DISPLAY" ]; then
    export QT_QPA_PLATFORM=wayland
else
    export QT_QPA_PLATFORM=xcb
fi

# Allow forcing a specific platform
if [ -n "\$QT_QPA_PLATFORM" ]; then
    echo "Using Qt platform: \$QT_QPA_PLATFORM"
fi

exec "\${HERE}/usr/bin/coretrace-qt" "\$@"
EOD

chmod +x /app/CTraceQt.AppDir/AppRun

# Copy the icon
mkdir -p /app/CTraceQt.AppDir/usr/share/icons/hicolor/256x256/apps/
cp /app/src/icons/coretrace.png /app/CTraceQt.AppDir/usr/share/icons/hicolor/256x256/apps/

# Use linuxdeployqt to handle all the dependencies
export VERSION=1.0
export QMAKE=/usr/bin/qmake
/app/linuxdeployqt-continuous-x86_64.AppImage /app/CTraceQt.AppDir/usr/share/applications/coretrace-qt.desktop \
    -appimage \
    -bundle-non-qt-libs \
    -extra-plugins=platforms/

# Move the AppImage to the output directory
mv CoreTrace*.AppImage /output/
EOF

chmod +x build-compile-appimage.sh

# Create Docker command to build the AppImage
cat > build-in-docker-full.sh << 'EOF'
#!/bin/bash
set -e

# Build the Docker image
docker build -t coretrace-appimage-builder-full .

# Run the container to build the AppImage
docker run --rm -it \
    -v $(pwd)/..:/src \
    -v $(pwd):/output \
    --device /dev/fuse --cap-add SYS_ADMIN \
    coretrace-appimage-builder-full /app/build-compile-appimage.sh
EOF

chmod +x build-in-docker-full.sh

echo "Scripts created. To build a compatible AppImage:"
echo "1. Make sure Docker is installed"
echo "2. Run: cd appimage-build-full && ./build-in-docker-full.sh"
