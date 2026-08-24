#!/bin/bash
set -e

BUILD_TYPE=Debug
STATIC_LINKING=ON
PROFILE=OFF
BUILD_DEMO=OFF
APP_TARGET=""
BUILD_DIR_BASE=_build/
BUILD_DIR=Debug
WEB=False

while [[ $# -gt 0 ]]; do
    case $1 in
        release|Release) BUILD_TYPE=Release; BUILD_DIR=Release; shift ;;
        debug|Debug)     BUILD_TYPE=Debug;   BUILD_DIR=Debug;   shift ;;
        static)          STATIC_LINKING=ON;  shift ;;
        dynamic)         STATIC_LINKING=OFF; shift ;;
        web|Web)         WEB=True;           BUILD_DIR=Web;      shift ;;
        profile)         PROFILE=ON;         shift ;;
        demo)            BUILD_DEMO=ON;      shift ;;
        --target)        APP_TARGET=$2;      shift ;;
        *) shift ;;
    esac
done

TARGET_ARGS=""
if [ -n "$APP_TARGET" ]; then TARGET_ARGS="--target $APP_TARGET"; fi

if [ "$WEB" = "True" ]; then
    echo "Building for Web..."
    if [ -n "$EMSDK" ]; then
        cmake -S . -B ${BUILD_DIR_BASE}${BUILD_DIR} -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DPLATFORM=Web -DBUILD_DEMO=$BUILD_DEMO \
            -DCMAKE_TOOLCHAIN_FILE="$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
    else
        emcmake cmake -S . -B ${BUILD_DIR_BASE}${BUILD_DIR} -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DPLATFORM=Web -DBUILD_DEMO=$BUILD_DEMO
    fi
    cmake --build ${BUILD_DIR_BASE}${BUILD_DIR} $TARGET_ARGS --parallel
else
    echo "Building $BUILD_TYPE configuration..."
    cmake -S . -B ${BUILD_DIR_BASE}${BUILD_DIR} -G "Unix Makefiles" -DIS_STATIC=$STATIC_LINKING -DPROFILE=$PROFILE -DBUILD_DEMO=$BUILD_DEMO -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    cmake --build ${BUILD_DIR_BASE}${BUILD_DIR} $TARGET_ARGS --config $BUILD_TYPE --parallel
fi
