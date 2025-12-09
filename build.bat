@echo off
REM Build script for anti-idle on Windows
REM Supports both MinGW (gcc) and MSVC (cl) compilers

setlocal enabledelayedexpansion

echo ========================================
echo Anti-Idle Build Script for Windows
echo ========================================
echo.

REM Set directories
set BUILD_DIR=build
set BIN_DIR=bin
set SRC_DIR=src
set TARGET=antiidle.exe
set SOURCE=%SRC_DIR%\antiidle.c

REM Check if source file exists
if not exist "%SOURCE%" (
    echo ERROR: Source file not found: %SOURCE%
    echo.
    pause
    exit /b 1
)

REM Create directories
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

echo Checking for C compiler...
echo.

REM Check for MinGW gcc in PATH first
where gcc >nul 2>&1
if %errorlevel% == 0 (
    echo [OK] Found GCC in PATH
    echo Compiling with MinGW GCC...
    gcc -Wall -Wextra -O2 -o "%BIN_DIR%\%TARGET%" "%SOURCE%" -luser32 -lgdi32
    if %errorlevel% == 0 (
        echo.
        echo ========================================
        echo Build successful!
        echo Binary created at: %BIN_DIR%\%TARGET%
        echo ========================================
        echo.
        goto :end
    ) else (
        echo [ERROR] Build failed with gcc.
        echo.
        goto :try_common_paths
    )
)

:try_common_paths
REM Check common MinGW installation paths
echo Checking common MinGW installation locations...

if exist "C:\mingw64\bin\gcc.exe" (
    echo [OK] Found GCC at: C:\mingw64\bin\gcc.exe
    echo Compiling with MinGW GCC...
    "C:\mingw64\bin\gcc.exe" -Wall -Wextra -O2 -o "%BIN_DIR%\%TARGET%" "%SOURCE%" -luser32 -lgdi32
    if %errorlevel% == 0 goto :success
)

if exist "C:\mingw32\bin\gcc.exe" (
    echo [OK] Found GCC at: C:\mingw32\bin\gcc.exe
    echo Compiling with MinGW GCC...
    "C:\mingw32\bin\gcc.exe" -Wall -Wextra -O2 -o "%BIN_DIR%\%TARGET%" "%SOURCE%" -luser32 -lgdi32
    if %errorlevel% == 0 goto :success
)

if exist "C:\msys64\mingw64\bin\gcc.exe" (
    echo [OK] Found GCC at: C:\msys64\mingw64\bin\gcc.exe
    echo Adding MSYS2 to PATH for DLL resolution...
    set "PATH=%PATH%;C:\msys64\mingw64\bin"
    echo Compiling with MinGW GCC...
    "C:\msys64\mingw64\bin\gcc.exe" -Wall -Wextra -O2 -o "%BIN_DIR%\%TARGET%" "%SOURCE%" -luser32 -lgdi32
    if !errorlevel! == 0 goto :success
    if %errorlevel% == 0 goto :success
)

if exist "C:\TDM-GCC-64\bin\gcc.exe" (
    echo [OK] Found GCC at: C:\TDM-GCC-64\bin\gcc.exe
    echo Compiling with MinGW GCC...
    "C:\TDM-GCC-64\bin\gcc.exe" -Wall -Wextra -O2 -o "%BIN_DIR%\%TARGET%" "%SOURCE%" -luser32 -lgdi32
    if %errorlevel% == 0 goto :success
)

goto :try_msvc

:success
echo.
echo ========================================
echo Build successful!
echo Binary created at: %BIN_DIR%\%TARGET%
echo ========================================
echo.
goto :end

:try_msvc
REM Check for MSVC cl compiler
where cl >nul 2>&1
if %errorlevel% == 0 (
    echo [OK] Found MSVC compiler
    echo Compiling with MSVC...
    cl /W3 /O2 /Fe:"%BIN_DIR%\%TARGET%" "%SOURCE%" user32.lib gdi32.lib /link /out:"%BIN_DIR%\%TARGET%"
    if %errorlevel% == 0 (
        echo.
        echo ========================================
        echo Build successful!
        echo Binary created at: %BIN_DIR%\%TARGET%
        echo ========================================
        echo.
        goto :end
    ) else (
        echo [ERROR] Build failed with MSVC cl.
        echo.
        goto :error
    )
) else (
    echo [INFO] MSVC compiler not found in PATH.
    echo.
)

:error
echo ========================================
echo ERROR: No suitable C compiler found!
echo ========================================
echo.
echo Please install one of the following:
echo.
echo OPTION 1: MinGW-w64 via Chocolatey (Easiest - Recommended)
echo   1. Install Chocolatey if you don't have it:
echo      Open PowerShell as Administrator and run:
echo      Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
echo   2. Install MinGW-w64:
echo      choco install mingw -y
echo   3. Close and reopen this command prompt
echo   4. Run build.bat again
echo.
echo OPTION 2: MinGW-w64 Manual Install
echo   1. Download from: https://winlibs.com/
echo      (Choose: "Win64 - GCC 13.2.0 + LLVM/Clang/LLD/LLDB 17.0.6 + MinGW-w64 11.0.1")
echo   2. Extract to C:\mingw64
echo   3. Add C:\mingw64\bin to your PATH environment variable
echo   4. Restart this command prompt and run build.bat again
echo.
echo OPTION 3: MSYS2 (Also Easy)
echo   1. Download from: https://www.msys2.org/
echo   2. Install and run: pacman -S mingw-w64-x86_64-gcc
echo   3. Add C:\msys64\mingw64\bin to your PATH
echo   4. Restart this command prompt and run build.bat again
echo.
echo OPTION 4: Visual Studio
echo   1. Install Visual Studio Community (free)
echo   2. Open "Developer Command Prompt for VS" from Start Menu
echo   3. Navigate to this directory and run build.bat
echo.
echo ========================================
echo.
pause
goto :end

:end
endlocal

