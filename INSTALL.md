# BML Installation Guide

BML is a C++17 matrix library built with CMake. It builds both static and shared libraries and has been tested with GCC, Clang and MSVC.

## Contents

- [Requirements](#requirements)
- [Getting the Source](#getting-the-source)
- [Configuring BML](#configuring-bml)
- [Build Options](#build-options)
- [Building BML](#building-bml)
- [Running the Tests](#running-the-tests)
- [Shared and Static Libraries](#shared-and-static-libraries)
- [LTO / IPO](#lto--ipo)
- [Installing BML](#installing-bml)
- [Using an Installed BML](#using-an-installed-bml)
- [Windows DLL Usage](#windows-dll-usage)
- [Custom Installation Prefix](#custom-installation-prefix)
- [Cleaning the Build](#cleaning-the-build)
- [Recommended Release Builds](#recommended-release-builds)
- [Troubleshooting](#troubleshooting)

---

# Requirements

## General

BML requires:

- C++17-compatible compiler
- CMake 3.18 or newer
- Git

BML is written in C++ and does not require a separate runtime or scripting language.

The following compilers have been tested:

- GCC
- Clang
- Microsoft Visual C++ (MSVC)

The following build systems have been tested:

- Ninja
- Make
- Visual Studio

---

## Linux

On Debian or Ubuntu, a basic build environment can be installed with:

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build git
```

Both GCC and Clang can be used.

To explicitly configure a build using Clang:

```bash
cmake -S . -B build \
    -G Ninja \
    -DCMAKE_CXX_COMPILER=clang++
```

---

## Windows

A Microsoft Visual Studio installation with C++ development support is required.

The Visual Studio installation should include:

- MSVC
- Windows SDK
- C++ build tools

CMake can automatically select an installed Visual Studio generator:

```powershell
cmake -S . -B build
```

Visual Studio generators are multi-configuration generators. `Debug` and `Release` are therefore selected when building rather than when configuring.

---

# Getting the Source

Clone the BML repository:

```bash
git clone <repository-url>
cd BML
```

Alternatively, download or extract a source release and enter the BML directory.

---

# Configuring BML

BML uses an out-of-source build. Generated files are placed in the `build` directory rather than in the source tree.

## Linux — Ninja

Configure a Release build:

```bash
cmake -S . -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release
```

Configure a Debug build:

```bash
cmake -S . -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug
```

## Windows — Visual Studio

Configure with:

```powershell
cmake -S . -B build
```

CMake will select the installed Visual Studio generator.

A multi-configuration build will contain configurations such as:

```text
build/
├── Debug/
└── Release/
```

---

# Build Options

BML provides several CMake options.

## BML_BUILD_SHARED

This option controls which library the `BML` CMake alias refers to.

Default:

```text
BML_BUILD_SHARED=ON
```

With the default setting:

```text
BML → BML_shared
```

To select the static library for the `BML` alias:

```bash
cmake -S . -B build \
    -DBML_BUILD_SHARED=OFF
```

This does **not** disable either library target. Both `BML_shared` and `BML_static` are created by the current build system.

The underlying targets can therefore be used independently:

```text
BML_shared
BML_static
```

---

## BML_RUN_TESTS

Controls whether the tests are run automatically as part of the normal build.

Default:

```text
BML_RUN_TESTS=ON
```

To enable automatic testing:

```bash
cmake -S . -B build \
    -DBML_RUN_TESTS=ON
```

To disable it:

```bash
cmake -S . -B build \
    -DBML_RUN_TESTS=OFF
```

When enabled, both the shared-library and static-library test programs are built and executed during the normal build.

---

## BML_ENABLE_LTO

> **Note:** This option is planned for the configurable build system. The current CMake configuration automatically enables IPO/LTO whenever the selected compiler supports it.

BML uses CMake's `CheckIPOSupported` module to determine whether interprocedural optimisation is supported.

When supported, LTO/IPO is enabled for:

- `BML_shared`
- `BML_static`
- `testMatrix_shared`
- `testMatrix_static`

The CMake configuration reports:

```text
-- Building with LTO
```

when LTO is available.

---

# Building BML

## Linux

After configuring:

```bash
cmake --build build
```

For a parallel build:

```bash
cmake --build build --parallel
```

If automatic tests are enabled, the tests will also be executed as part of the build.

## Windows

Build Debug:

```powershell
cmake --build build --config Debug
```

Build Release:

```powershell
cmake --build build --config Release
```

For a parallel Release build:

```powershell
cmake --build build --config Release --parallel
```

---

# Running the Tests

BML contains functional and stress tests covering the supported matrix types and operations.

The test programs are:

```text
testMatrix_shared
testMatrix_static
```

When `BML_RUN_TESTS=ON`, they are executed automatically during the normal build.

They can also be run manually.

## Linux

```bash
./build/testMatrix_shared
./build/testMatrix_static
```

## Windows

Release:

```powershell
.\build\Release\testMatrix_shared.exe
.\build\Release\testMatrix_static.exe
```

Debug:

```powershell
.\build\Debug\testMatrix_shared.exe
.\build\Debug\testMatrix_static.exe
```

Both tests should complete successfully.

---

# Shared and Static Libraries

BML builds both a shared and a static library.

The actual CMake targets are:

```text
BML_shared
BML_static
```

The `BML` target is an alias which selects one of these according to `BML_BUILD_SHARED`.

## Linux

The shared library is:

```text
libBML.so
```

The static library is:

```text
libBML.a
```

## Windows

The shared library consists of:

```text
BML.dll
BML.lib
```

`BML.dll` contains the actual shared-library implementation.

`BML.lib` is the MSVC import library used when linking an application against the DLL.

The static library is:

```text
BML_static.lib
```

When statically linking, the BML implementation is included directly in the application executable.

---

# LTO / IPO

BML uses CMake's interprocedural optimisation mechanism when supported by the selected compiler.

IPO is commonly known as link-time optimisation (LTO).

The implementation uses:

```cmake
include(CheckIPOSupported)

check_ipo_supported(...)
```

to determine whether the compiler supports the required optimisation.

Different compilers implement this differently:

- GCC uses GCC LTO.
- Clang uses LLVM LTO.
- MSVC uses link-time code generation (LTCG).

LTO can make intermediate object files and static libraries considerably larger.

The size of a static library is therefore not necessarily representative of the size of the final executable.

The final optimisation takes place when the executable or shared library is linked.

---

# Installing BML

BML uses CMake's standard installation mechanism together with `GNUInstallDirs`.

After building BML, install it with:

```bash
sudo cmake --install build
```

The default installation prefix is normally:

```text
/usr/local
```

On a typical Linux installation, the result is approximately:

```text
/usr/local/
├── include/
│   └── BML/
└── lib/
    ├── libBML.so
    └── libBML.a
```

The exact library directory is determined by CMake.

Both the shared and static libraries are installed.

The public headers below:

```text
include/
```

are installed below:

```text
/usr/local/include/BML/
```

You can see what CMake intends to install without actually installing anything:

```bash
cmake --install build --dry-run
```

---

# Using an Installed BML

After installation, BML headers can be included from a C++ program:

```cpp
#include <BML/Matrix.hpp>
```

For example:

```cpp
#include <iostream>
#include <BML/Matrix.hpp>

int main()
{
    bml::Matrix<double> A(3, 3);

    A.fill(1.0);

    bml::Matrix<double> B = A * 2.0;
    bml::Matrix<double> C = A + B;

    std::cout << C << '\n';

    return 0;
}
```

A simple Linux compilation using the default installation prefix is:

```bash
g++ -std=c++17 main.cpp \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lBML \
    -o myprogram
```

For a non-standard installation prefix, replace the include and library paths accordingly.

---

# Static Linking

To link against the static library on Linux:

```bash
g++ -std=c++17 main.cpp \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lBML \
    -o myprogram
```

If both `libBML.so` and `libBML.a` are available, the linker normally prefers the shared library.

To explicitly select the static library:

```bash
g++ -std=c++17 main.cpp \
    -I/usr/local/include \
    /usr/local/lib/libBML.a \
    -o myprogram
```

On Windows, link against:

```text
BML_static.lib
```

The resulting executable does not require `BML.dll` for BML itself.

---

# Shared Linking

On Linux:

```bash
g++ -std=c++17 main.cpp \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lBML \
    -o myprogram
```

The resulting program requires:

```text
libBML.so
```

at runtime.

If `/usr/local/lib` is not already known to the dynamic linker, update the linker cache:

```bash
sudo ldconfig
```

Alternatively, for temporary testing:

```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

---

# Windows DLL Usage

A Windows application using the shared BML library requires:

```text
BML.dll
```

at runtime.

The application is linked against:

```text
BML.lib
```

A typical deployment therefore contains:

```text
myprogram.exe
BML.dll
```

The import library:

```text
BML.lib
```

is required when linking the application but normally does not need to be distributed with the finished application.

For static linking, use:

```text
BML_static.lib
```

and no BML DLL is required at runtime.

---

# Windows Symbol Exports

BML uses explicit symbol visibility for Windows shared-library builds.

Public API declarations use:

```cpp
BML_API
```

The implementation is defined in:

```text
include/BML/export.hpp
```

When building the Windows DLL, the shared-library target is compiled with:

```text
BML_BUILDING_DLL
```

This causes public symbols to be exported using:

```cpp
__declspec(dllexport)
```

Consumers of the DLL use:

```text
BML_USE_DLL
```

so that the declarations use:

```cpp
__declspec(dllimport)
```

Public classes, functions and operators that form part of the shared-library API must therefore have the appropriate `BML_API` declaration.

---

# Inspecting the Windows DLL

Visual Studio provides `dumpbin` for inspecting generated binaries.

To list exported DLL symbols:

```powershell
dumpbin /exports BML.dll
```

To search for BML symbols:

```powershell
dumpbin /exports BML.dll |
    Select-String "Matrix|BoolRef|RowView|StringStorage"
```

To inspect the static library:

```powershell
dumpbin /linkermember:2 BML_static.lib
```

To inspect the import library:

```powershell
dumpbin /linkermember:2 BML.lib
```

---

# Custom Installation Prefix

The installation prefix can be changed during configuration.

For example:

```bash
cmake -S . -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$HOME/.local"
```

Build and install:

```bash
cmake --build build
cmake --install build
```

The resulting installation will be placed below:

```text
$HOME/.local/
```

This allows BML to be installed without root privileges.

Another possible installation prefix is:

```text
/opt/BML
```

For example:

```bash
cmake -S . -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/opt/BML
```

Then:

```bash
cmake --build build
sudo cmake --install build
```

---

# Cleaning the Build

To completely remove the generated build directory:

## Linux

```bash
rm -rf build
```

## Windows PowerShell

```powershell
Remove-Item -Recurse -Force build
```

A clean configuration can then be created again:

```bash
cmake -S . -B build
```

A clean build is recommended after changing the compiler or fundamental CMake configuration.

---

# Recommended Release Builds

## Linux

For a clean Release build using Ninja:

```bash
rm -rf build

cmake -S . -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBML_BUILD_SHARED=ON \
    -DBML_RUN_TESTS=ON

cmake --build build --parallel
```

The build will automatically enable LTO if the selected compiler supports it.

Run the tests manually if required:

```bash
./build/testMatrix_shared
./build/testMatrix_static
```

Then install:

```bash
sudo cmake --install build
```

## Windows

For a clean Visual Studio Release build:

```powershell
Remove-Item -Recurse -Force build

cmake -S . -B build `
    -DBML_BUILD_SHARED=ON `
    -DBML_RUN_TESTS=ON

cmake --build build --config Release --parallel
```

Run the tests:

```powershell
.\build\Release\testMatrix_shared.exe
.\build\Release\testMatrix_static.exe
```

Inspect the generated DLL if required:

```powershell
dumpbin /exports .\build\Release\BML.dll
```

---

# Troubleshooting

## CMake cannot find a compiler

Verify that the compiler is installed and available.

For GCC:

```bash
g++ --version
```

For Clang:

```bash
clang++ --version
```

On Windows, configure from a Visual Studio Developer PowerShell or use a Visual Studio generator supported by the installed CMake.

---

## LTO is not available

BML checks compiler support before enabling IPO/LTO.

If IPO is unsupported, CMake reports the reason and builds without LTO.

LTO is an optimisation feature and is not required for BML to function correctly.

---

## Windows DLL symbols are missing

Inspect the DLL's export table:

```powershell
dumpbin /exports BML.dll
```

Public API classes and functions that cross the DLL boundary must be declared with:

```cpp
BML_API
```

Also verify that the `BML_shared` target is being built with:

```text
BML_BUILDING_DLL
```

---

## Tests fail after changing the build configuration

Perform a clean build.

Linux:

```bash
rm -rf build
```

Windows:

```powershell
Remove-Item -Recurse -Force build
```

Then configure and build again.

---

# Summary

A standard Linux Release build is:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

A standard Windows Release build is:

```powershell
cmake -S . -B build
cmake --build build --config Release --parallel
```

Install on Linux with:

```bash
sudo cmake --install build
```

BML builds both shared and static libraries:

```text
Linux:
    libBML.so
    libBML.a

Windows:
    BML.dll
    BML.lib
    BML_static.lib
```

Both the shared and static test programs should be run successfully before considering a build ready for installation.
