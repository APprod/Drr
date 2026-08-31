#!/bin/bash

set -e

BUILD_TYPE=Debug
STATIC_LINKING=ON
PROFILE=OFF
BUILD_DEMO=OFF
APP_TARGET=""
BUILD_DIR_BASE="_build/"
BUILD_DIR=Debug
WEB=False
CMAKE_ARGS=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        release|Release) BUILD_TYPE=Release; BUILD_DIR=Release; shift ;;
        debug|Debug) BUILD_TYPE=Debug; BUILD_DIR=Debug; shift ;;
        static) STATIC_LINKING=ON; shift ;;
        dynamic) STATIC_LINKING=OFF; shift ;;
        web|Web) WEB=True; BUILD_DIR=Web; shift ;;
        profile) PROFILE=ON; shift ;;
        demo) BUILD_DEMO=ON; shift ;;
        --target)
            [[ -z "$2" ]] && { echo "Error: --target requires an argument"; exit 1; }
            APP_TARGET="$2"
            shift 2
            ;;
        *) CMAKE_ARGS+=("$1"); shift ;;
    esac
done

TARGET_ARGS=()
if [[ -n "$APP_TARGET" ]]; then TARGET_ARGS=(--target "$APP_TARGET"); fi

if [[ "$WEB" == "True" ]]; then
    echo "Building for Web..."

    CMAKE_ARGS_BASE=(-S . -B "${BUILD_DIR_BASE}${BUILD_DIR}" -G "Unix Makefiles" "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" "-DPLATFORM=Web" "-DBUILD_DEMO=$BUILD_DEMO")

    if [[ -n "$EMSDK" ]]; then
        CMAKE_ARGS_BASE+=("-DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
        cmake "${CMAKE_ARGS_BASE[@]}" "${CMAKE_ARGS[@]}"
    else
        emcmake cmake "${CMAKE_ARGS_BASE[@]}" "${CMAKE_ARGS[@]}"
    fi

    cmake --build "${BUILD_DIR_BASE}${BUILD_DIR}" "${TARGET_ARGS[@]}" --parallel
else
    echo "Building $BUILD_TYPE configuration..."

    CMAKE_ARGS_BASE=(-S . -B "${BUILD_DIR_BASE}${BUILD_DIR}" -G "Unix Makefiles" "-DIS_STATIC=$STATIC_LINKING" "-DPROFILE=$PROFILE" "-DBUILD_DEMO=$BUILD_DEMO" "-DCMAKE_BUILD_TYPE=$BUILD_TYPE")

    cmake "${CMAKE_ARGS_BASE[@]}" "${CMAKE_ARGS[@]}"
    cmake --build "${BUILD_DIR_BASE}${BUILD_DIR}" "${TARGET_ARGS[@]}" --config "$BUILD_TYPE" --parallel
fi