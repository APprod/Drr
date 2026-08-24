#!/bin/bash
set -e

# Creates a ready app project from the ApEngine template.
# Usage (run from your project repo root): ./ApEngine/create.sh [<appName>] [--ci]
# Mirrors create.bat behaviour for Linux/macOS.

APP_NAME=""
CI=0
TEMPLATE_DIR="$(cd "$(dirname "$0")" && pwd)/template"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --ci) CI=1; shift ;;
        -*) echo "[create] Unknown argument \"$1\""; shift ;;
        *) if [ -z "$APP_NAME" ]; then APP_NAME="$1"; shift; else echo "[create] Unknown argument \"$1\""; shift; fi ;;
    esac
done

if [ -z "$APP_NAME" ]; then
    if [ "$CI" = "1" ]; then
        echo "[create] No app name given - CI only mode."
    else
        echo "Usage: create.sh [<appName>] [--ci]"
        echo "Run from your project repo root. Creates ./apps/<appName> from the"
        echo "ApEngine template and copies root scripts only where they are missing."
        echo "Use --ci to also copy GitHub CI workflows from template/ci."
        echo "Use --ci alone to only copy CI workflows without creating an app."
        exit 1
    fi
fi

if [ -n "$APP_NAME" ]; then
if ! [[ "$APP_NAME" =~ ^[A-Za-z0-9_-]+$ ]]; then
    echo "[create] Invalid app name \"$APP_NAME\" - use letters, digits, _ or -."
    exit 1
fi

if [ ! -d "$TEMPLATE_DIR/app" ]; then
    echo "[create] Template not found near create.sh: $TEMPLATE_DIR/app"
    exit 1
fi

if [ -e "apps/$APP_NAME" ]; then
    echo "[create] apps/$APP_NAME already exists - aborting."
    exit 1
fi

echo "[create] Copying template app to apps/$APP_NAME ..."
mkdir -p "apps"
cp -r "$TEMPLATE_DIR/app" "apps/$APP_NAME"

COPIED_ROOT_CMAKE=0
for F in CMakeLists.txt build.bat run.bat build.sh .gitignore; do
    if [ -f "$TEMPLATE_DIR/$F" ]; then
        if [ -f "$F" ]; then
            echo "[create] Root $F already exists - keeping yours."
        else
            cp "$TEMPLATE_DIR/$F" "./$F"
            echo "[create] Copied $F to repo root."
            if [ "$F" = "CMakeLists.txt" ]; then COPIED_ROOT_CMAKE=1; fi
        fi
    fi
done

# Register the new app in a pre-existing root CMakeLists.txt (idempotent).
if [ "$COPIED_ROOT_CMAKE" = "0" ]; then
    if ! grep -Fq "add_subdirectory(apps/$APP_NAME)" "CMakeLists.txt" 2>/dev/null; then
        echo "" >> "CMakeLists.txt"
        echo "add_subdirectory(apps/$APP_NAME)" >> "CMakeLists.txt"
        echo "[create] Appended add_subdirectory(apps/$APP_NAME) to root CMakeLists.txt."
    fi
fi

# Fresh root CMakeLists still points at the template folder - retarget it.
if [ "$COPIED_ROOT_CMAKE" = "1" ]; then
    sed -i "s|apps/template|apps/$APP_NAME|g" "CMakeLists.txt"
    sed -i "s/project(AppTest)/project($APP_NAME)/" "CMakeLists.txt"
fi
fi

if [ "$CI" = "1" ]; then
    if [ ! -d "$TEMPLATE_DIR/ci" ]; then
        echo "[create] --ci requested but template/ci not found - skipping."
    else
        mkdir -p ".github/workflows"
        for F in "$TEMPLATE_DIR/ci"/*; do
            [ -e "$F" ] || continue
            bn=$(basename "$F")
            if [ -e ".github/workflows/$bn" ]; then
                echo "[create] .github/workflows/$bn already exists - keeping yours."
            else
                cp "$F" ".github/workflows/"
                echo "[create] Copied ci/$bn to .github/workflows/"
            fi
        done
    fi
fi

if [ -n "$APP_NAME" ]; then
    echo "[create] Done: apps/$APP_NAME"
    echo "[create] Build it with:  ./build.sh debug static --target $APP_NAME"
    echo "[create] Or on Windows:   build.bat debug static --target $APP_NAME"
else
    echo "[create] Done: CI workflows installed."
fi
