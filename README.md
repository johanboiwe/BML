# BML

This is BML. It stands for **Boiwe Matrix Library**. It is a 2D matrix library that seeks to imitate some of NumPy's functionality. It's written in C++17.

Some of the functionality and documentation have been created with AI assistance, but everything has been checked by a true, living *Homo sapiens sapiens* (a hairless ape that lost his tail).

The library has been tested on true metal Debian 13 x86-64. In virtual machines it has been tested on,
* Windows 11
* Debian 12 i686
* Tiny Core Linux i486
* Debian 13 arm64
* Debian 13 ppc64
* Debian 13 risc-v64

Via github actions it has also been tested on:
* Mac OS


## Supported data types

BML supports the following data types:

* int8_t
* uint8_t
* int16_t
* uint16_t
* int32_t
* uint32_t
* int64_t
* uint64_t
* float
* double
* long double
* half precision float (from the Half-Therock library)
* char
* bool
* std::string
* Json
* void* — generic pointers for storing references to arbitrary objects

Note: Serialisation and deserialisation use the host architecture's native endianness. The size and representation of float are platform-dependent.
## Installation

See the [installation guide](INSTALL.md) for more thorough instructions.

A basic installation can be performed with:

```bash
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

You will need:

* A C++ compiler supporting C++17. GCC, Clang and MSVC have been tested.
* CMake 3.18 or greater.
* A supported build system. Make and Ninja have been tested.

## Example

```cpp
#include <iostream>
#include <bml/bml.hpp>

int main()
{
    bml::Matrix<double> A(3, 3);

    A.fill(1.0);

    bml::Matrix<double> B = A * 2.0;
    bml::Matrix<double> C = A + B;

    std::cout << "A:\n" << A << '\n';
    std::cout << "B:\n" << B << '\n';
    std::cout << "C:\n" << C << '\n';

    C *= 0.5;

    std::cout << "C after *= 0.5:\n" << C << '\n';

    return 0;
}
```

Output:

```text
A:
1 1 1
1 1 1
1 1 1

B:
2 2 2
2 2 2
2 2 2

C:
3 3 3
3 3 3
3 3 3

C after *= 0.5:
1.5 1.5 1.5
1.5 1.5 1.5
1.5 1.5 1.5
```

## Documentation

See the documentation for information about the available matrix operations, data types and API.

## Project status

BML is under active development. The API may change between versions.

## Licence

See [LICENSE](LICENSE.md).
## Acknowledgements

BML uses [nlohmann/json](https://github.com/nlohmann/json) for JSON support.

nlohmann/json is licensed under the MIT License. The library is vendored in BML under `external/nlohmann/`.

