# Installation Guide (v1.1.0)

This guide explains how to configure, build, and install the library on Linux/macOS and Windows.

- POSIX (Linux/macOS): use `./configure` (Bash).
- Windows: use `configure.bat` (CMD).

Both scripts generate a `Makefile` that you can build with `make` (or `nmake` if you prefer on Windows).

---

## 1) Linux / macOS (`./configure`)

### Prerequisites
- **GNU Make**
- **Clang++** (default) or **G++**
- A 64‑bit toolchain. (64‑bit is enforced; you can choose a CPU baseline with `--arch`.)

### Key defaults
- **Library**: `both` (static **and** shared)
- **Build type**: Release (`--enable-debug` switches to debug)
- **LTO**: `full` (set `--enable-lto=thin` on Clang, or `--disable-lto`)
- **Linker**: prefers `lld` when using Clang (disable with `--no-lld`)
- **Compiler**: `clang++`

### Options
```
--lib=static|shared|both     Select library type (default: both)
--enable-debug               Debug build (-O1 -g -fno-omit-frame-pointer)
--disable-debug              Release build (default)
--enable-lto[=full|thin]     Enable LTO (default: full; thin = Clang only)
--disable-lto                Disable LTO
--no-lld                     Do not use lld even with Clang
--compiler=<clang++|g++>     Choose compiler (default: clang++)
--arch=<baseline>            CPU baseline, e.g. native, x86-64-v2, x86-64-v3
--prefix=<dir>               Install prefix (default: /usr/local)
```
> Note: 64‑bit builds are enforced via `-m64`.

### Typical usage
```bash
./configure --lib=both --enable-lto=thin --compiler=clang++ --arch=x86-64-v3
make -j
sudo make install
```

---

## 2) Windows (`configure.bat`)

### Prerequisites
- **Visual Studio C++ Build Tools** (CL) or **clang-cl**
- **Make**: You can use `make` (e.g. from MSYS2/MinGW) or `nmake`. The generated `Makefile` works with standard `make`.

### Key defaults
- **Library**: `both` (static **and** shared)
- **Build type**: Release
- **LTO**: `full`
- **Linker**: prefers `lld` when using `clang-cl`
- **Compiler**: `clang-cl` (you may switch to `cl`)
- **Install prefix**: `C:\Program Files\MyLibrary`

### Options
```
--lib=static|shared|both     Select library type (default: both)
--enable-debug               Debug build
--disable-debug              Release build (default)
--enable-lto[=full|thin]     Enable LTO (default: full; thin = clang-cl only)
--disable-lto                Disable LTO
--no-lld                     Do not use lld with clang-cl
--compiler=<cl|clang-cl>     Choose compiler (default: clang-cl)
--arch=<native|x86-64-v2|x86-64-v3>
                            CPU baseline (maps to /arch or -march when applicable)
--prefix=<dir>               Install prefix (default: C:\Program Files\MyLibrary)
```

### Notes on flags
- **MSVC (cl)**: LTO uses `/GL` with `/LTCG` at link; `/std:c++17`, `/W4`, `/O2` in release.
- **clang-cl**: LTO uses `-flto[=thin]`; when available and allowed, `lld-link` is selected via `-fuse-ld=lld`.

### Typical usage (PowerShell or CMD)
```cmd
configure.bat --lib=both --enable-lto=thin --compiler=clang-cl --arch=x86-64-v3
make
make install
```

---

## 3) Targets in the generated Makefile
- `all` (default): builds the selected library type
- `static`: builds static library only
- `shared`: builds shared library only
- `both`: builds both
- `install`: installs headers and libraries into `--prefix`
- `clean`: removes objects and built artefacts

---

## 4) Versioning
- **Library version**: 1.1.0
- **SOVERSION**: 1

---

## 5) Troubleshooting
- Ensure the selected compiler is on your `PATH`.
- On Linux/macOS with Clang, if `lld` isn’t present or you pass `--no-lld`, the system linker will be used.
- On Windows, if using `clang-cl`, install *LLVM* (for `lld-link`) or pass `--no-lld`.
