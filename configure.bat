@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

REM Default settings
set "LIB_TYPE=static"
set "ENABLE_DEBUG=false"
set "COMPILER=cl"  REM Default compiler
set "ARCH=x64"  REM Default to 64-bit architecture
set "PREFIX=C:\Program Files\MyLibrary"  REM Default installation directory
set "INSTALL_BOTH=false"  REM Do not install both by default

REM Help message
:show_help
echo Usage: configure.bat [options]
echo.
echo Options:
echo   --enable-shared       Build shared library
echo   --enable-static       Build static library (default)
echo   --enable-debug        Enable debug build with debug symbols
echo   --enable-32bit        Build for 32-bit architecture
echo   --enable-both         Build both static and shared libraries
echo   --prefix=^<dir^>        Specify installation directory (default: C:\Program Files\MyLibrary)
echo   --compiler=^<compiler^> Specify the compiler to use (default: cl)
echo   --help                Display this help message
EXIT /B 0

REM Parse command line arguments
set "ARGS=%*"
for %%A in (%ARGS%) do (
    if "%%A"=="--enable-shared" (
        set "LIB_TYPE=shared"
    ) else if "%%A"=="--enable-static" (
        set "LIB_TYPE=static"
    ) else if "%%A"=="--enable-debug" (
        set "ENABLE_DEBUG=true"
    ) else if "%%A"=="--enable-32bit" (
        set "ARCH=x86"
    ) else if "%%A"=="--enable-both" (
        set "INSTALL_BOTH=true"
    ) else if "%%~A"=="--help" (
        goto show_help
    ) else (
        echo %%~A | findstr /B /C:"--prefix=" >nul
        if not errorlevel 1 (
            set "PREFIX=%%~A"
            set "PREFIX=!PREFIX:--prefix=!"
        ) else (
            echo %%~A | findstr /B /C:"--compiler=" >nul
            if not errorlevel 1 (
                set "COMPILER=%%~A"
                set "COMPILER=!COMPILER:--compiler=!"
            ) else (
                echo Unknown option: %%A
                goto show_help
            )
        )
    )
)

REM Check if the compiler exists
where /Q "%COMPILER%"
if errorlevel 1 (
    echo Error: Compiler '%COMPILER%' not found. Please install it or specify a different compiler using --compiler option.
    EXIT /B 1
)

REM Set architecture flags
if "%ARCH%"=="x86" (
    set "ARCH_FLAGS=/arch:IA32"
) else (
    set "ARCH_FLAGS=/arch:AVX"
)

REM Set debug flags
if "%ENABLE_DEBUG%"=="true" (
    set "DEBUG_FLAGS=/Zi /Od"
) else (
    set "DEBUG_FLAGS=/O2 /DNDEBUG"
)

REM Check if the compiler can compile a simple program
echo int main() { return 0; } > test_compile.cpp
"%COMPILER%" /nologo %ARCH_FLAGS% test_compile.cpp /Fe:test_compile.exe >nul 2>&1
if errorlevel 1 (
    echo Error: Compiler '%COMPILER%' failed to compile a simple test program for %ARCH%.
    del test_compile.cpp
    EXIT /B 1
)
del test_compile.cpp test_compile.exe test_compile.obj

REM Check if the compiler can link a shared library
echo int test() { return 0; } > test_link.cpp
"%COMPILER%" /nologo %ARCH_FLAGS% test_link.cpp /LD /Fe:test_link.dll >nul 2>&1
if errorlevel 1 (
    echo Error: Compiler '%COMPILER%' failed to link a shared library for %ARCH%.
    del test_link.cpp
    EXIT /B 1
)
del test_link.cpp test_link.dll test_link.exp test_link.lib test_link.obj

REM Generate the NMake Makefile based on the options
(
echo # Auto-generated Makefile
echo.
echo CC = %COMPILER%
echo CXXFLAGS = /EHsc /W3 /Iinclude %ARCH_FLAGS% %DEBUG_FLAGS%
echo.
echo SOURCES = source\matrix.cpp source\testMatrix.cpp
echo OBJECTS = $(SOURCES:.cpp=.obj)
echo.
echo LIB_DIR = lib
echo STATIC_LIB = $(LIB_DIR)\BML.lib
echo SHARED_LIB = $(LIB_DIR)\BML.dll
echo.
echo LIB_TYPE = %LIB_TYPE%
echo INSTALL_BOTH = %INSTALL_BOTH%
echo PREFIX = %PREFIX%
echo.
echo all: $(LIB_TYPE)
echo.
echo static: $(STATIC_LIB)
echo.
echo $(STATIC_LIB): $(OBJECTS)
echo ^	if not exist $(LIB_DIR) mkdir $(LIB_DIR)
echo ^	lib /OUT:$(STATIC_LIB) $(OBJECTS)
echo.
echo shared: $(SHARED_LIB)
echo.
echo $(SHARED_LIB): $(OBJECTS)
echo ^	if not exist $(LIB_DIR) mkdir $(LIB_DIR)
echo ^	$(CC) $(OBJECTS) /LD /Fe$(SHARED_LIB)
echo.
echo .cpp.obj:
echo ^	$(CC) $(CXXFLAGS) /c $< /Fo$@
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
echo ^	del /Q $(LIB_DIR)\*.*
echo ^	del /Q $(OBJECTS)
) > Makefile

REM Output summary
echo Configuration complete.
echo Library type: %LIB_TYPE%
echo Debugging enabled: %ENABLE_DEBUG%
echo Compiler: %COMPILER%
echo Architecture: %ARCH%
echo Install both static and shared libraries: %INSTALL_BOTH%
echo Install prefix: %PREFIX%

EXIT /B 0

