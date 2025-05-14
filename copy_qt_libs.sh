#!/bin/bash

# Create the Qt plugins directory if it doesn't exist
mkdir -p CTraceQt.AppDir/usr/lib/qt/plugins/platforms/
mkdir -p CTraceQt.AppDir/usr/lib/qt/plugins/wayland-graphics-integration/
mkdir -p CTraceQt.AppDir/usr/lib/qt/plugins/wayland-decoration-client/
mkdir -p CTraceQt.AppDir/usr/lib/qt/plugins/wayland-shell-integration/

# Copy the Wayland platform plugin
cp /usr/lib/qt/plugins/platforms/libqwayland-*.so CTraceQt.AppDir/usr/lib/qt/plugins/platforms/

# Copy the Wayland integration plugins
cp /usr/lib/qt/plugins/wayland-graphics-integration-*/* CTraceQt.AppDir/usr/lib/qt/plugins/wayland-graphics-integration/
cp /usr/lib/qt/plugins/wayland-decoration-client/* CTraceQt.AppDir/usr/lib/qt/plugins/wayland-decoration-client/
cp /usr/lib/qt/plugins/wayland-shell-integration/* CTraceQt.AppDir/usr/lib/qt/plugins/wayland-shell-integration/

# Copy Wayland-specific libraries
cp /usr/lib/libwayland-client.so* CTraceQt.AppDir/usr/lib/
cp /usr/lib/libwayland-cursor.so* CTraceQt.AppDir/usr/lib/
cp /usr/lib/libwayland-egl.so* CTraceQt.AppDir/usr/lib/
cp /usr/lib/libxkbcommon.so* CTraceQt.AppDir/usr/lib/

# Modify the AppRun script to handle Wayland
cat > CTraceQt.AppDir/AppRun << EOF
#!/bin/bash
HERE="\$(dirname "\$(readlink -f "\${0}")")"
export PATH="\${HERE}/usr/bin:\${PATH}"
export LD_LIBRARY_PATH="\${HERE}/usr/lib:\${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="\${HERE}/usr/lib/qt/plugins"
export QML2_IMPORT_PATH="\${HERE}/usr/lib/qml"
export QT_QPA_PLATFORM_PLUGIN_PATH="\${HERE}/usr/lib/qt/plugins/platforms"

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
EOF

chmod +x CTraceQt.AppDir/AppRun
