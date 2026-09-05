$BUILD_TYPE = "Debug"
$STATIC_LINKING = "ON"
$BUILD_DIR_BASE = "_build\"
$BUILD_DIR = "Debug"
$WEB = $false
$MSVC = $false
$CLANG = $false
$BUILD_DEMO = "OFF"
$PROFILE = "OFF"
$BS_THREAD = "OFF"
$APP_NAME = ""

$START_TIME = Get-Date

$CMakeArgs = [System.Collections.Generic.List[string]]::new()

$i = 0;
while ($i -lt $args.Count){
    $arg = $args[$i]
    switch ($arg.ToString().ToLower()) {
        "--target" {
            $APP_NAME = $args[++$i]
        }
        "release" { $BUILD_TYPE = "Release"; $BUILD_DIR = "Release" }
        "debug" { $BUILD_TYPE = "Debug"; $BUILD_DIR = "Debug" }
        "static" { $STATIC_LINKING = "ON" }
        "dynamic" { $STATIC_LINKING = "OFF" }
        "web" { $WEB = $true; $BUILD_DIR_BASE = "_build\Web\" }
        "msvc" { $MSVC = $true }
        "clang" { $CLANG = $true }
        "profile" { $PROFILE = "ON" }
        "bsthread" { $BS_THREAD = "ON" }
        "demo" { $BUILD_DEMO = "ON" }
        default {
            $CMakeArgs.Add($arg)
        }
    }
    $i++
}

if ($MSVC) {
    $BUILD_DIR = "msvc-$BUILD_DIR"
}

$TargetArgs = @()

if ($APP_NAME) {
    $TargetArgs = @("--target", $APP_NAME)
}

if ($WEB) {
    Write-Host "Building $BUILD_TYPE configuration... For Web"

    if ($env:EMSDK) {
        & cmake -S . -B "$BUILD_DIR_BASE$BUILD_DIR" -G "MinGW Makefiles" "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" `
            "-DPLATFORM=Web" "-DBUILD_DEMO=$BUILD_DEMO" "-DBS_THREAD=$BS_THREAD" "-DCMAKE_TOOLCHAIN_FILE=$env:EMSDK\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake" `
            @CMakeArgs
    }
    else {
        & emcmake cmake -S . -B "$BUILD_DIR_BASE$BUILD_DIR" -G "MinGW Makefiles" "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" "-DPLATFORM=Web" "-DBUILD_DEMO=$BUILD_DEMO" "-DBS_THREAD=$BS_THREAD" @CMakeArgs
    }

    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }

    & cmake --build "$BUILD_DIR_BASE$BUILD_DIR" @TargetArgs --parallel

    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}
elseif ($MSVC) {
    Write-Host "Building $BUILD_TYPE configuration... MSVC"

    & cmake -S . -B "$BUILD_DIR_BASE$BUILD_DIR" -G "Visual Studio 17 2022" -A x64 "-DIS_STATIC=$STATIC_LINKING" "-DPROFILE=$PROFILE" "-DBUILD_DEMO=$BUILD_DEMO" "-DBS_THREAD=$BS_THREAD" @CMakeArgs

    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }

    & cmake --build "$BUILD_DIR_BASE$BUILD_DIR" @TargetArgs --config $BUILD_TYPE --parallel

    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}
else {
    Write-Host "Building $BUILD_TYPE configuration..."

    if ($CLANG) {
        & cmake -S . -B "$BUILD_DIR_BASE$BUILD_DIR" -G "MinGW Makefiles" "-DCMAKE_C_COMPILER=clang" "-DCMAKE_CXX_COMPILER=clang++" "-DIS_STATIC=$STATIC_LINKING" "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" "-DPROFILE=$PROFILE" "-DBUILD_DEMO=$BUILD_DEMO" "-DBS_THREAD=$BS_THREAD" @CMakeArgs
    }
    else {
        & cmake -S . -B "$BUILD_DIR_BASE$BUILD_DIR" -G "MinGW Makefiles" "-DIS_STATIC=$STATIC_LINKING" "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" "-DPROFILE=$PROFILE" "-DBUILD_DEMO=$BUILD_DEMO" "-DBS_THREAD=$BS_THREAD" @CMakeArgs
    }

    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }

    & cmake --build "$BUILD_DIR_BASE$BUILD_DIR" @TargetArgs --config $BUILD_TYPE --parallel

    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}

$END_TIME = Get-Date
$elapsed = $END_TIME - $START_TIME

Write-Host ""
Write-Host ("Total time: {0:hh\:mm\:ss\.fff} s" -f $elapsed)

$global:ApEngineBuildConfig = @{
    APP_NAME       = $APP_NAME
    BUILD_DIR      = $BUILD_DIR
    BUILD_DIR_BASE = $BUILD_DIR_BASE
    WEB            = $WEB
}
return $global:ApEngineBuildConfig