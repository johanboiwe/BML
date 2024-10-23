Installation Guide
Overview

This guide explains how to configure, build, and install the software library on both Linux (or macOS) and Windows systems. 
The `configure` script (Bash) is intended for Posix environments (like Linux, Unix and MacOS), while `configure.bat` is used for Windows with a command-line interface.

1. Installation on Linux or macOS (Using Bash `configure` Script)
Prerequisites:

- GNU Make: Required to process the generated Makefile.
- Clang++ or GCC: A C++ compiler. Clang++ is the default, but you can change it to GCC if needed.
- Development Tools: Ensure you have tools such as `libtool`, `make`, and `autoconf` installed.
- 64-bit or 32-bit System: The script defaults to a 64-bit system but can be switched to 32-bit.

Steps:
    • Download the Project: Extract the project files or clone them from your version control system.
    • Navigate to the Project Directory:
    • Run the Configure Script: The `configure` script accepts several options to customize the build:
    • - `--enable-shared`: Build a shared library (dynamic).
    • - `--enable-static`: Build a static library (default).
    • - `--enable-debug`: Enable debug mode.
    • - `--enable-32bit`: Build for a 32-bit architecture.
    • - `--enable-both`: Build both static and shared libraries.
    • - `--prefix=<dir>`: Specify the installation directory (default: `/usr/local`).
    • - `--compiler=<compiler>`: Specify the compiler to use (default: `clang++`).
    • Check the Output: The script will generate a Makefile.
    • Build the Project: After configuration, run `make`.
    • Install the Library: Once built, install the library with `sudo make install`.
    • Cleaning Up: To remove object files and binaries, run `make clean`.
2. Installation on Windows (Using `configure.bat`)
Prerequisites:

- NMake: The Microsoft make utility, often included with Visual Studio.
- Visual Studio C++ Compiler: This script uses `cl.exe` (Microsoft's C++ compiler), but you can change this.

Steps:
    • Download the Project: Extract or clone the project files.
    • Open the Command Prompt and navigate to the project directory.
    • Run the Configuration Script: The `configure.bat` script accepts several options.
    • - `--enable-shared`: Build a shared library (`.dll`).
    • - `--enable-static`: Build a static library (`.lib`).
    • - `--enable-debug`: Enable debugging symbols.
    • - `--enable-32bit`: Build for a 32-bit architecture.
    • - `--enable-both`: Build both static and shared libraries.
    • - `--prefix=<path>`: Specify the installation directory.
    • - `--compiler=<compiler>`: Specify the C++ compiler.
    • Check the Output: The script will generate an NMake-compatible Makefile.
    • Build the Project: Once configured, build the project using `nmake`.
    • Install the Library: Run `nmake install` to copy the compiled libraries and header files.
    • Cleaning Up: To remove the build artifacts, run `nmake clean`.
3. Options Explained

- Shared vs Static Libraries:
  - Static libraries (`.lib` or `.a`) are compiled directly into your executable.
  - Shared libraries (`.dll`, `.so`, or `.dylib`) are loaded dynamically at runtime.
- Debugging: The `--enable-debug` option includes debugging symbols.
- 32-bit Architecture: Use the `--enable-32bit` option to build for a 32-bit system.
- Prefix: The `--prefix=<path>` option allows you to specify a custom installation directory.
