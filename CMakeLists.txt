cmake_minimum_required(VERSION 3.20)
project(BML VERSION 2.0.0 LANGUAGES CXX)

# ---------- C++ standard ----------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The main library
set(BML_LIB_SOURCES
        src/instantiations.cpp
        src/testMatrix.cpp
)

# ---------- Libraries ----------
add_library(BML_static STATIC ${BML_LIB_SOURCES})
add_library(BML_shared SHARED ${BML_LIB_SOURCES})
set(BML_TARGETS BML_static BML_shared)