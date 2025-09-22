@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

REM ============================================================
REM Defaults (mirroring Linux ./configure)
REM ============================================================
set "LIB_TYPE=both"              REM static | shared | both
set "INSTALL_BOTH=true"          REM install both by default
set "ENABLE_DEBUG=false"         REM release by default
set "ENABLE_LTO=full"            REM off | full | thin  (thin requires clang/lld)
set "USE_LLD=true"               REM prefer lld when using clang-cl if available
set "ARCH="                      REM e.g. native, x86-64-v2, x86-64-v3 (mapped)
set "PREFIX=C:\Program Files\MyLibrary"
set "COMPILER=clang-cl"          REM or cl
set "VERSION=1.1.0"
set "SOVERSION=1"

REM ============================================================
REM Help
REM ============================================================
if "%~1"=="" goto parse_args
:show_help
echo Usage: configure.bat [options]
echo.
echo Build options:
echo   --lib=static^|shared^|both     Library type ^(default: both^)
echo   --enable-debug                 Debug build ^(-O1 /Od, symbols^)
echo   --disable-debug                Release build ^(default^)
echo   --enable-lto[=full^|thin]      Enable LTO ^(default: full; thin=clang only^)
echo   --disable-lto                  Disable LTO
echo   --no-lld                       Do not use lld, even with clang
echo   --compiler=<cxx>               cl ^| clang-cl  ^(default: clang-cl^)
echo   --arch=<val>                   CPU baseline: native, x86-64-v2, x86-64-v3
echo.
echo Install:
echo   --prefix=<dir>                 Install prefix ^(default: C:\Program Files\MyLibrary^)
echo.
echo Example:
echo   configure.bat --lib=both --enable-lto=thin --compiler=clang-cl --arch=x86-64-v3
EXIT /B 0

REM ============================================================
REM Parse arguments
REM ============================================================
:parse_args
for %%A in (%*) do (
    for /F "tokens=1,2 delims==" %%K in ("%%~A") do (
        if /I "%%~K"=="--lib" (
            if /I "%%~L"=="static" (set "LIB_TYPE=static") ^
            else if /I "%%~L"=="shared" (set "LIB_TYPE=shared") ^
            else if /I "%%~L"=="both" (set "LIB_TYPE=both") ^
            else (
                echo Unknown --lib value: %%~L
                goto show_help
            )
        ) else if /I "%%~K"=="--enable-debug" (
            set "ENABLE_DEBUG=true"
        ) else if /I "%%~K"=="--disable-debug" (
            set "ENABLE_DEBUG=false"
        ) else if /I "%%~K"=="--enable-lto" (
            if "%%~L"=="" (set "ENABLE_LTO=full") else (set "ENABLE_LTO=%%~L")
        ) else if /I "%%~K"=="--disable-lto" (
            set "ENABLE_LTO=off"
        ) else if /I "%%~K"=="--no-lld" (
            set "USE_LLD=false"
        ) else if /I "%%~K"=="--compiler" (
            set "COMPILER=%%~L"
        ) else if /I "%%~K"=="--arch" (
            set "ARCH=%%~L"
        ) else if /I "%%~K"=="--prefix" (
            set "PREFIX=%%~L"
        ) else if "%%~K"=="--help" (
            goto show_help
        ) else (
            REM Accept combined form --option=value as well
            echo %%~A | findstr /B /C:"--compiler=" >nul && (
                for /F "tokens=2 delims==" %%Z in ("%%~A") do set "COMPILER=%%~Z"
                goto :continue_args
            )
            echo %%~A | findstr /B /C:"--prefix=" >nul && (
                for /F "tokens=2 delims==" %%Z in ("%%~A") do set "PREFIX=%%~Z"
                goto :continue_args
            )
            echo %%~A | findstr /B /C:"--arch=" >nul && (
                for /F "tokens=2 delims==" %%Z in ("%%~A") do set "ARCH=%%~Z"
                goto :continue_args
            )
            echo %%~A | findstr /B /C:"--enable-lto=" >nul && (
                for /F "tokens=2 delims==" %%Z in ("%%~A") do set "ENABLE_LTO=%%~Z"
                goto :continue_args
            )
            echo Unknown option: %%~A
            goto show_help
        )
        :continue_args
    )
)

REM ============================================================
REM Normalise choices
REM ============================================================
if /I "%LIB_TYPE%"=="both" (set "INSTALL_BOTH=true") else (set "INSTALL_BOTH=false")

REM ============================================================
REM Detect toolchain kind
REM ============================================================
set "IS_MSVC=false"
set "IS_CLANG=false"
echo %COMPILER% | findstr /I /R "^cl(\.exe)?$" >nul && set "IS_MSVC=true"
echo %COMPILER% | findstr /I "clang" >nul && set "IS_CLANG=true"

where /Q "%COMPILER%"
if errorlevel 1 (
    echo error: compiler '%COMPILER%' not found
    EXIT /B 1
)

REM ============================================================
REM Flags
REM ============================================================
set "CXXFLAGS="
set "LDFLAGS="
set "DEFINES="
set "ARCH_FLAGS="
set "LTO_CXX="
set "LTO_LD="

REM Map --arch to Windows flags (approximate)
if /I "%ARCH%"=="x86-64-v3" (
    if "%IS_MSVC%"=="true" ( set "ARCH_FLAGS=/arch:AVX2" ) else ( set "ARCH_FLAGS=-m64 -march=x86-64-v3" )
) else if /I "%ARCH%"=="x86-64-v2" (
    if "%IS_MSVC%"=="true" ( set "ARCH_FLAGS=" ) else ( set "ARCH_FLAGS=-m64 -march=x86-64-v2" )
) else if /I "%ARCH%"=="native" (
    if "%IS_MSVC%"=="true" ( set "ARCH_FLAGS=" ) else ( set "ARCH_FLAGS=-m64 -march=native" )
) else (
    if "%IS_MSVC%"=="true" ( set "ARCH_FLAGS=" ) else ( set "ARCH_FLAGS=-m64" )
)

if "%ENABLE_DEBUG%"=="true" (
    if "%IS_MSVC%"=="true" (
        set "CXXFLAGS=/std:c++17 /W4 /EHsc /permissive- /Od /Zi /FS /DDEBUG /MDd"
        set "LDFLAGS=/DEBUG"
    ) else (
        set "CXXFLAGS=-std=c++17 -Wall -Wextra -Wpedantic -O1 -g -fno-omit-frame-pointer"
    )
) else (
    if "%IS_MSVC%"=="true" (
        set "CXXFLAGS=/std:c++17 /W4 /EHsc /permissive- /O2 /DNDEBUG /MD"
    ) else (
        set "CXXFLAGS=-std=c++17 -Wall -Wextra -Wpedantic -O3"
    )
)

REM LTO selection
if /I "%ENABLE_LTO%"=="off" (
    REM nothing
) else if /I "%ENABLE_LTO%"=="thin" (
    if "%IS_CLANG%"=="true" (
        set "LTO_CXX=-flto=thin"
        set "LTO_LD=-flto=thin"
    ) else (
        echo warning: thin LTO requested but not using clang; falling back to full LTO
        if "%IS_MSVC%"=="true" ( set "LTO_CXX=/GL" & set "LTO_LD=/LTCG" ) else ( set "LTO_CXX=-flto" & set "LTO_LD=-flto" )
    )
) else (
    if "%IS_MSVC%"=="true" ( set "LTO_CXX=/GL" & set "LTO_LD=/LTCG" ) else ( set "LTO_CXX=-flto" & set "LTO_LD=-flto" )
)

REM Prefer lld with clang if requested
set "LDSEL="
if "%IS_CLANG%"=="true" if /I "%USE_LLD%"=="true" (
    where /Q lld-link.exe
    if not errorlevel 1 ( set "LDSEL=-fuse-ld=lld" )
)

REM Compose final flags
if "%IS_MSVC%"=="true" (
    set "CXXCMD=%COMPILER% /nologo %CXXFLAGS% %ARCH_FLAGS%"
    set "LIB_EXT=lib"
    set "SO_EXT=dll"
    set "AR=lib"
) else (
    set "CXXCMD=%COMPILER% %CXXFLAGS% %ARCH_FLAGS% %LTO_CXX% %LDSEL%"
    set "LIB_EXT=a"
    set "SO_EXT=dll"
    set "AR=llvm-lib"
)

REM ============================================================
REM Generate Makefile (mirrors Linux variables where sensible)
REM ============================================================
set "LIBDIR=lib"
set "OBJDIR=.objs"
set "TARGET=MyLibrary"
set "STATIC_LIB=%LIBDIR%\%TARGET%.%LIB_EXT%"
set "SHARED_LIB=%LIBDIR%\%TARGET%.%SO_EXT%"

if not exist "%OBJDIR%" mkdir "%OBJDIR%"
if not exist "%LIBDIR%" mkdir "%LIBDIR%"

(
echo # Auto-generated by configure.bat
echo CXX=%CXXCMD%
echo AR=%AR%
echo CXXFLAGS=
echo LDFLAGS=%LTO_LD%
echo PREFIX=%PREFIX%
echo OBJDIR=%OBJDIR%
echo LIBDIR=%LIBDIR%
echo TARGET=%TARGET%
echo STATIC_LIB=%STATIC_LIB%
echo SHARED_LIB=%SHARED_LIB%
echo LIB_TYPE=%LIB_TYPE%
echo INSTALL_BOTH=%INSTALL_BOTH%
echo.
echo SOURCES= ^
echo 	src\main.cpp ^
echo 	src\instantiations.cpp
echo.
echo OBJECTS=$(SOURCES:src\=%.objs\=^).obj
echo.
echo all: $(LIB_TYPE)
echo.
echo static:
echo ^	if not exist $(LIBDIR) mkdir $(LIBDIR)
echo ^	$(CXX) $(SOURCES) /c /Fo$(OBJDIR)\
echo ^	$(AR) /OUT:$(STATIC_LIB) $(OBJDIR)\*.obj
echo.
echo shared:
echo ^	if not exist $(LIBDIR) mkdir $(LIBDIR)
echo ^	$(CXX) $(SOURCES) /c /Fo$(OBJDIR)\
echo ^	$(CXX) $(OBJDIR)\*.obj /LD %LDFLAGS% /Fe:$(SHARED_LIB)
echo.
echo both: static shared
echo.
echo install: $(LIB_TYPE)
echo ^	if not exist "$(PREFIX)\lib" mkdir "$(PREFIX)\lib"
echo ^	if not exist "$(PREFIX)\include" mkdir "$(PREFIX)\include"
echo ^	if "$(INSTALL_BOTH)"=="true" (
echo ^		if exist $(STATIC_LIB) copy /Y $(STATIC_LIB) "$(PREFIX)\lib\"
echo ^		if exist $(SHARED_LIB) copy /Y $(SHARED_LIB) "$(PREFIX)\lib\"
echo ^	) else (
echo ^		if "$(LIB_TYPE)"=="static" if exist $(STATIC_LIB) copy /Y $(STATIC_LIB) "$(PREFIX)\lib\"
echo ^		if "$(LIB_TYPE)"=="shared" if exist $(SHARED_LIB) copy /Y $(SHARED_LIB) "$(PREFIX)\lib\"
echo ^	)
echo ^	copy /Y include\*.hpp "$(PREFIX)\include\"
echo.
echo clean:
echo ^	if exist $(OBJDIR) del /Q $(OBJDIR)\*.obj
echo ^	if exist $(LIBDIR) del /Q $(LIBDIR)\*.*
) > Makefile

REM ============================================================
REM Summary
REM ============================================================
echo.
echo Configuration complete.
echo   Library type:     %LIB_TYPE%
echo   Debug enabled:    %ENABLE_DEBUG%
echo   LTO:              %ENABLE_LTO%
echo   Use lld:          %USE_LLD%
echo   Compiler:         %COMPILER%  ^(msvc=!IS_MSVC! clang=!IS_CLANG!^)
echo   Arch:             %ARCH%
echo   Install both:     %INSTALL_BOTH%
echo   Prefix:           %PREFIX%

EXIT /B 0
