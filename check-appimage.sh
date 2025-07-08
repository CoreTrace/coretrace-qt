#!/bin/bash

# Script pour vérifier rapidement si l'AppImage est générée
# Usage: ./check-appimage.sh

echo "🔍 Vérification rapide de l'AppImage..."

# Vérifier si l'AppImage existe
APPIMAGE_PATH="appimage-build-full/CoreTrace_Qt-1.0-x86_64.AppImage"

if [ ! -f "$APPIMAGE_PATH" ]; then
    echo "❌ AppImage non trouvée à: $APPIMAGE_PATH"
    echo "💡 Lancez d'abord: ./app-image-builder.sh puis cd appimage-build-full && ./build-in-docker-full.sh"
    exit 1
fi

echo "✅ AppImage trouvée!"
echo ""

# Informations sur l'AppImage
echo "📊 Informations:"
echo "📁 Chemin: $APPIMAGE_PATH"
echo "📏 Taille: $(du -h "$APPIMAGE_PATH" | cut -f1)"
echo "🔍 Type: $(file "$APPIMAGE_PATH")"
echo "✅ Permissions: $(ls -la "$APPIMAGE_PATH" | cut -d' ' -f1,3,4)"

# Tester si exécutable
if [ -x "$APPIMAGE_PATH" ]; then
    echo "▶️  Exécutable: OUI"
else
    echo "⚠️  Exécutable: NON (chmod +x requis)"
fi

echo ""
echo "✅ Vérification terminée avec succès!"
