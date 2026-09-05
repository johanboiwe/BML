# BML

This is BML. It stands for **Boiwe Matrix Library**. It is a 2D matrix library that seeks to imitate some of NumPy's functionality. It's written in C++17.

Some of the functionality and documentation have been created with AI assistance, but everything has been checked by a true, living *Homo sapiens sapiens* (a hairless ape that lost his tail).

The library has been tested on Debian 11 and 13, FreeBSD 15, and Windows 11. MacOS has not been tested.

## Supported data types

BML supports the following data types:

* 'int8_t'
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
* char
* bool
* std::string
* void* — generic pointers for storing references to arbitrary objects

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

See [LICENSE](LICENSE).
