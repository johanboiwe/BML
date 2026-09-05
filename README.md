BML

This is BML. It stands for Boiwe Matrix Library. It is a 2D matrix library that seeks to imitate some of NumPy's functionality. It's written in C++ 17.

Some of the functionality and documentation have been created with AI assistance, but everything has been checked by a true, living Homo sapiens sapiens (a hairless ape that lost his tail).

The library has been tested on Debian 11 and 13, FreeBSD 15, and Windows 11. MacOS has not been tested.

The library supports several data types as:
- int8_t
- uint8_t
- int16_t
- uint16_t
- int32_t
- uint32_t
- int64_t
- uint64_t
- float
- double
- long double
- char
- bool
- std::string
- void*


Se the installation guide for more thorough instructions.
To install the library:
cmake -S . -B build
cmake --build build
cmake --target install

You will need a C++ compiler supporting C++17 (GCC, Clang and MCVC has been tested), Cmake 3.18 or greater, a buildsystem (Make and ninja has been tested).

Example code:

#include <bml/bml.hpp>

int main(){
  
