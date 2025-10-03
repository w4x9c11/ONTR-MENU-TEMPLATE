@echo off
setlocal

set CONFIG=%1

if "%CONFIG%"=="" (
    echo Usage: build.bat [Debug^|Release]
    exit /b 1
)

cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=%CONFIG%

REM 执行构建
cmake --build build --config %CONFIG%

echo ===============================
echo Build %CONFIG% Success.
echo ===============================