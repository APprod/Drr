#!/bin/bash
set -e

./build.sh "$@"

if echo "$*" | grep -qw "web"; then
    emrun _build/Web/app.html
else
    EXE=_build/Debug/app
    [ -f "$EXE" ] || EXE=_build/Release/app
    "$EXE"
fi
