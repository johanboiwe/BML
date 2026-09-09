# Building and Installing BML

BML is a C++17 matrix library built with CMake. The project builds both static and shared versions of the library, together with separate test programs for each version.

BML supports the four standard CMake build configurations:

- **Debug** — debugging enabled with minimal optimisation.
- **Release** — optimised production build. This is BML's default.
- **RelWithDebInfo** — optimised build with debugging information.
- **MinSizeRel** — optimised primarily for reducing binary size.

The way the build configuration is selected depends on the CMake generator. Single-configuration generators such as Ninja use `CMAKE_BUILD_TYPE`, while multi-configuration generators such as Visual Studio and Xcode select the configuration when the project is built.

## Requirements

The project requires:

- CMake 3.18 or newer
- A C++17-compatible compiler
- Ninja, Make, or another supported CMake build tool

The C++ standard is configured by CMake:

- C++17 is required.
- Compiler-specific C++ extensions are disabled.

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

## Build type

If no build type is specified, BML defaults to **Release**:

```cmake
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
```

Therefore, a normal single-configuration CMake build produces a Release build unless another build type is explicitly selected.

For example, to explicitly configure a Debug build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

For Release:

```bash
cmake -S . -B build -G Ninja
```

The explicit Release option is optional because Release is already the default.

The other standard configurations can be selected in the same way:

```bash
cmake -S . -B build-relwithdebinfo -G Ninja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

```bash
cmake -S . -B build-minsizerel -G Ninja \
    -DCMAKE_BUILD_TYPE=MinSizeRel
```

## Building with Ninja

Configure the project:

```bash
cmake -S . -B build -G Ninja
```

Build it:

```bash
cmake --build build --parallel
```

Because BML defaults to Release, the first command is sufficient for a normal Release build.

For other configurations, specify `CMAKE_BUILD_TYPE` during configuration.

## MacOS

MacOS uses Apple's development tools and has a somewhat different setup from Linux distributions. Homebrew is useful for installing CMake and Ninja, while the normal MacOS compiler is Apple Clang.

### Install Apple's Command Line Tools

Install the Xcode Command Line Tools if they are not already installed:

```bash
xcode-select --install
```

This provides the Apple compiler and SDK required for building C++ software.

Check that Clang is available:

```bash
clang++ --version
```

On a normal MacOS installation, `clang++` is Apple's Clang.

### Install Homebrew

If Homebrew is not already installed, install it using the official Homebrew installation instructions. After installation, make sure Homebrew's environment is loaded into the shell configuration.

Check that it works:

```bash
brew --version
```

### Install CMake and Ninja

The easiest way to install the CMake build tools is through Homebrew:

```bash
brew install cmake ninja
```

Verify the installations:

```bash
cmake --version
ninja --version
```

### Build BML with Apple Clang

The recommended MacOS build is to let CMake use the system Apple Clang and use Ninja as the build system:

```bash
cmake -S . -B build -G Ninja
```

Then build:

```bash
cmake --build build --parallel
```

Run the tests:

```bash
ctest --test-dir build --output-on-failure
```

This produces the normal MacOS shared-library form:

```text
libBML.dylib
```

and the static library:

```text
libBML.a
```

### Check which compiler CMake selected

CMake records the compiler in the build directory. You can inspect it with:

```bash
grep CMAKE_CXX_COMPILER build/CMakeCache.txt
```

For a normal Apple Clang build, it will point to the MacOS Clang installation.

You can also configure explicitly with Apple Clang:

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++
```

Normally this is unnecessary; CMake will find the compiler automatically.

### Homebrew LLVM

Homebrew also provides a separate LLVM/Clang toolchain:

```bash
brew install llvm
```

However, Homebrew's LLVM is **keg-only**, because MacOS already provides its own LLVM-based compiler toolchain. Homebrew therefore does not simply replace `/usr/bin/clang++` with its version.

If you specifically want to build BML using Homebrew LLVM, obtain its installation prefix:

```bash
brew --prefix llvm
```

Then configure CMake explicitly. For example:

```bash
LLVM_PREFIX="$(brew --prefix llvm)"

cmake -S . -B build-llvm -G Ninja \
    -DCMAKE_C_COMPILER="$LLVM_PREFIX/bin/clang" \
    -DCMAKE_CXX_COMPILER="$LLVM_PREFIX/bin/clang++"
```

Build it with:

```bash
cmake --build build-llvm --parallel
```

Using a separate build directory is recommended when switching compiler toolchains:

```text
build/
build-llvm/
```

This prevents CMake from accidentally reusing compiler settings from a previous configuration.

### Apple Silicon and Intel Macs

The same CMake commands work on both Intel Macs and Apple Silicon Macs.

You can check the architecture with:

```bash
uname -m
```

Typical results are:

```text
x86_64
```

for Intel Macs, and:

```text
arm64
```

for Apple Silicon.

On Apple Silicon, it is generally preferable to use a native ARM64 Homebrew installation and native ARM64 compiler rather than running the entire build through Rosetta.

### MacOS Debug build

To create a Debug build:

```bash
cmake -S . -B build-debug -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug
```

Build it:

```bash
cmake --build build-debug --parallel
```

Run the tests:

```bash
ctest --test-dir build-debug --output-on-failure
```

Using a separate directory makes it easy to keep Release and Debug builds simultaneously:

```text
build/
build-debug/
```

### MacOS installation

A user-local installation is often more convenient on MacOS than installing directly into `/usr/local`.

For example:

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_INSTALL_PREFIX="$HOME/.local"
```

Build and install:

```bash
cmake --build build --parallel
cmake --install build
```

The headers will be installed under:

```text
$HOME/.local/include/BML
```

and the libraries under:

```text
$HOME/.local/lib
```

The CMake package files will be installed under:

```text
$HOME/.local/lib/cmake/BML
```

If another CMake project needs to find this installation, the prefix can be supplied through `CMAKE_PREFIX_PATH`:

```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$HOME/.local"
```

For a project that uses BML:

```cmake
find_package(BML REQUIRED)
```

The installed BML targets are exported under the `BML::` namespace.

## Windows

Windows has some important differences from Linux and MacOS, particularly when using Visual Studio.

The recommended Windows compiler is **Microsoft Visual C++ (MSVC)** through Visual Studio.

### Visual Studio

Visual Studio uses a **multi-configuration** CMake generator. Unlike Ninja, all standard configurations can be generated into the same build directory.

For Visual Studio 2022:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

The architecture can be changed if required, but `x64` is the normal choice for a modern 64-bit Windows build.

After configuration, select the desired configuration when building:

```powershell
cmake --build build --config Debug
```

```powershell
cmake --build build --config Release
```

```powershell
cmake --build build --config RelWithDebInfo
```

```powershell
cmake --build build --config MinSizeRel
```

This is different from a Ninja build.

With Ninja:

```text
Configure:
    -DCMAKE_BUILD_TYPE=Release

Build:
    cmake --build build
```

With Visual Studio:

```text
Configure:
    cmake -S . -B build -G "Visual Studio 17 2022"

Build:
    cmake --build build --config Release
```

The `CMAKE_BUILD_TYPE` setting is therefore not used to select the active configuration with the Visual Studio generator.

### Visual Studio Developer PowerShell

When using MSVC directly with command-line tools such as Ninja, the MSVC compiler environment must be initialised.

The easiest approach is to use the **Developer PowerShell for Visual Studio** or **Developer Command Prompt for Visual Studio**.

Inside such a shell, verify that the compiler is available:

```powershell
cl
```

You can then use Ninja:

```powershell
cmake -S . -B build -G Ninja
cmake --build build --parallel
```

Using the Visual Studio generator is generally simpler if you do not specifically need Ninja.

### Windows library files

Windows uses a different naming scheme for shared libraries.

BML produces:

```text
BML.dll
```

for the shared library itself.

The corresponding import library is:

```text
BML.lib
```

The static library is:

```text
BML_static.lib
```

These `.lib` files serve different purposes:

```text
BML.lib
    Import library for BML.dll

BML_static.lib
    Static version of BML
```

They are **not interchangeable**.

This distinction is particularly important when linking another Windows application against BML.

### Running the shared-library test

The shared test executable is:

```text
testMatrix_shared.exe
```

It links against the shared BML library and therefore requires:

```text
BML.dll
```

to be available at runtime.

If the executable cannot find the DLL, Windows will report that the required module could not be found.

When manually copying the executable outside the build directory, copy the required `BML.dll` with it or otherwise make the DLL available through the normal Windows DLL search path.

After installation, the DLL is installed into the CMake runtime directory:

```text
bin/
```

while the import and static libraries are installed into:

```text
lib/
```

A typical installed layout is therefore:

```text
bin/
    BML.dll

lib/
    BML.lib
    BML_static.lib

include/
    BML/
        ...
```

### MSVC-specific configuration

The BML CMake configuration contains several MSVC-specific warning suppressions:

```text
C4804
C4805
C4244
```

These are applied to both `BML_shared` and `BML_static`.

When building the shared library on Windows, BML also defines:

```text
BML_BUILDING_DLL
```

This allows the BML headers to distinguish between building the DLL and using the DLL.

### Windows with Ninja

Ninja can also be used with MSVC:

```powershell
cmake -S . -B build -G Ninja
cmake --build build --parallel
```

However, the MSVC developer environment must be active so that `cl.exe` and the associated linker and SDK tools are available.

For a straightforward Windows build, the Visual Studio generator is usually the simpler option:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
```

## Libraries

BML creates two library targets:

```text
BML_static
BML_shared
```

The static library is built from:

```text
src/instantiations.cpp
```

The shared library uses the same source.

The public headers are taken from:

```text
include/
```

### Static library

The static target is:

```text
BML_static
```

On Linux and other non-Windows platforms, its output name is:

```text
libBML.a
```

On Windows, its output name is:

```text
BML_static
```

### Shared library

The shared target is:

```text
BML_shared
```

Its output name is:

```text
BML
```

The exact filename produced by the compiler/platform is platform-dependent.

For example, on Linux this results in the usual shared-library naming convention:

```text
libBML.so
```

On MacOS, the corresponding shared-library form is normally:

```text
libBML.dylib
```

On Windows, the shared library is normally produced as:

```text
BML.dll
```

with its associated import library:

```text
BML.lib
```

## The `BML` target

BML provides a convenient `BML` alias.

The CMake option controlling the alias is:

```cmake
option(BML_BUILD_SHARED "Build shared library" ON)
```

It defaults to `ON`.

When enabled:

```text
BML → BML_shared
```

When disabled:

```text
BML → BML_static
```

For example:

```bash
cmake -S . -B build -G Ninja -DBML_BUILD_SHARED=OFF
```

makes `BML` refer to the static library.

This option does **not** prevent the underlying `BML_shared` or `BML_static` target from being created. Both library targets are defined by the project; the option determines which one the `BML` alias represents.

## Tests

BML contains two separate test executables.

### Shared-library test

```text
testMatrix_shared
```

This is built from:

```text
src/testMatrix.cpp
```

and links against:

```text
BML_shared
```

### Static-library test

```text
testMatrix_static
```

This also uses:

```text
src/testMatrix.cpp
```

but links against:

```text
BML_static
```

This allows the same test program to be tested against both forms of the library.

## CTest

Testing is enabled with:

```cmake
enable_testing()
```

Two CTest tests are registered:

```text
Matrix_Shared
Matrix_Static
```

After building, run the complete test suite with:

```bash
ctest --test-dir build
```

For verbose test output:

```bash
ctest --test-dir build --output-on-failure
```

The two CTest tests execute:

```text
testMatrix_shared
testMatrix_static
```

respectively.

## LTO / IPO

BML automatically checks whether the compiler supports CMake's interprocedural optimisation (IPO) facility:

```cmake
include(CheckIPOSupported)

check_ipo_supported(
    RESULT BML_IPO_SUPPORTED
    OUTPUT BML_IPO_ERROR
    LANGUAGES CXX
)
```

If supported, IPO is enabled for all four main build targets:

```text
BML_static
BML_shared
testMatrix_shared
testMatrix_static
```

CMake reports:

```text
Building with LTO
```

when this is enabled.

If the compiler does not support IPO/LTO, CMake does not abort the build. Instead, it produces a warning:

```text
LTO/IPO is not supported: ...
```

and the project continues without IPO.

There is currently no separate `BML_ENABLE_LTO` option. LTO is automatically selected when supported by the compiler and build environment.

## Installation

The project uses CMake's standard installation directory variables from:

```cmake
include(GNUInstallDirs)
```

The main installation locations are therefore platform-aware.

The libraries are installed to:

```text
${CMAKE_INSTALL_LIBDIR}
```

Executables/DLL runtime files are installed to:

```text
${CMAKE_INSTALL_BINDIR}
```

Headers are installed to:

```text
${CMAKE_INSTALL_INCLUDEDIR}/BML
```

On a typical Linux installation with the default `/usr/local` prefix, this corresponds to locations such as:

```text
/usr/local/include/BML
/usr/local/lib
```

Install the project with:

```bash
cmake --install build
```

A different installation prefix can be selected during configuration:

```bash
cmake -S . -B build -G Ninja -DCMAKE_INSTALL_PREFIX=$HOME/.local
```

and then:

```bash
cmake --install build
```

## CMake package support

BML installs a CMake package configuration so that another CMake project can find the installed library.

The package files are installed under:

```text
${CMAKE_INSTALL_LIBDIR}/cmake/BML
```

The installed package contains:

```text
BMLConfig.cmake
BMLConfigVersion.cmake
BMLTargets.cmake
```

The exported targets use the namespace:

```text
BML::
```

The package version is generated from the project version:

```text
2.0.1
```

and uses:

```text
COMPATIBILITY SameMajorVersion
```

This means compatible BML 2.x releases can be treated as belonging to the same major-version compatibility range.

A consuming CMake project can therefore use the installed package with:

```cmake
find_package(BML REQUIRED)
```

and link against the exported BML targets.

## Doxygen

CMake also processes the project's Doxygen configuration:

```cmake
configure_file(
    Doxyfile
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)
```

This creates a configured copy of `Doxyfile` in the build directory.

The CMake configuration shown here does **not** define a Doxygen build target itself. Documentation generation is handled separately from the library build.

## Useful CMake commands

### Ninja

Configure:

```bash
cmake -S . -B build -G Ninja
```

Build:

```bash
cmake --build build --parallel
```

Run tests:

```bash
ctest --test-dir build
```

Run tests and show failures:

```bash
ctest --test-dir build --output-on-failure
```

Install:

```bash
cmake --install build
```

Configure a Debug build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

Configure a `RelWithDebInfo` build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

Configure a `MinSizeRel` build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel
```

Build using the static `BML` alias:

```bash
cmake -S . -B build -G Ninja -DBML_BUILD_SHARED=OFF
```

Configure with a custom installation prefix:

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_INSTALL_PREFIX=$HOME/.local
```

### Visual Studio

Configure a 64-bit Visual Studio build:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

Build Release:

```powershell
cmake --build build --config Release --parallel
```

Build Debug:

```powershell
cmake --build build --config Debug --parallel
```

Build `RelWithDebInfo`:

```powershell
cmake --build build --config RelWithDebInfo --parallel
```

Build `MinSizeRel`:

```powershell
cmake --build build --config MinSizeRel --parallel
```

Run the tests for a specific Visual Studio configuration:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

Install a specific Visual Studio configuration:

```powershell
cmake --install build --config Release
```

## Build structure

The important targets are:

```text
BML_static
    └── libBML.a / BML_static.lib / platform equivalent

BML_shared
    └── libBML.so / libBML.dylib / BML.dll

BML
    └── alias to BML_shared or BML_static

testMatrix_shared
    └── links to BML_shared

testMatrix_static
    └── links to BML_static
```

The project therefore provides both library variants while allowing users and downstream CMake projects to select the preferred `BML` target.
