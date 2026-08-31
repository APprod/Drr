$config = & "$PSScriptRoot\build.ps1" @args

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

if (-not $config.APP_NAME) {
    Write-Host "Usage: run.ps1 --target <app_name> [debug|release|static|dynamic|web|msvc|profile]"
    exit 1
}

if ($config.WEB) {
    if($config.APP_NAME = demo){
        & "$($config.BUILD_DIR_BASE)$($config.BUILD_DIR)\ApEngine\examples\demo\demo.exe"    
    }else{
        & emrun "$($config.BUILD_DIR_BASE)$($config.BUILD_DIR)\apps\$($config.APP_NAME)\$($config.APP_NAME).html"
    }
}
else {
    if($config.APP_NAME -eq "demo"){
        & "$($config.BUILD_DIR_BASE)$($config.BUILD_DIR)\ApEngine\examples\demo\demo.exe"    
    }else{
        & "$($config.BUILD_DIR_BASE)$($config.BUILD_DIR)\apps\$($config.APP_NAME)\$($config.APP_NAME).exe"
    }
}