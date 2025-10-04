cmake_minimum_required(VERSION 3.20)

project(BML VERSION 2.0.0 LANGUAGES CXX)

# ---------- C++ standard ----------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------- Options ----------
option(BML_ENABLE_WARNINGS "Enable extra warnings" ON)
option(BML_ENABLE_LTO      "Enable IPO/LTO for the libraries" ON)
option(BML_INSTALL_PACKAGE "Install CMake package config" ON)

# ---------- Default build type ----------
set(DEFAULT_BUILD_TYPE "RelWithDebInfo")
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Build type" FORCE)
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

include(GNUInstallDirs)

# ---------- Sources ----------
# Only compile implementation TUs into the libraries.
set(BML_LIB_SOURCES
        src/instantiations.cpp
        src/boolRef.cpp
)

# Test executable owns only test code; it links the shared lib.
set(BML_TEST_SOURCES
        src/main.cpp
        src/testMatrix.cpp
)

# ---------- Libraries ----------
add_library(BML_static STATIC ${BML_LIB_SOURCES})
add_library(BML_shared SHARED ${BML_LIB_SOURCES})

# Both named libBML.*
set_target_properties(BML_static PROPERTIES OUTPUT_NAME BML)
set_target_properties(BML_shared PROPERTIES OUTPUT_NAME BML)
set_target_properties(BML_shared PROPERTIES
        VERSION   ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Public includes for consumers
target_include_directories(BML_static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(BML_shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Tell headers we are building the lib (guards extern template, etc.)
target_compile_definitions(BML_static PRIVATE BML_BUILDING=1)
target_compile_definitions(BML_shared PRIVATE BML_BUILDING=1)

# ---------- Warnings ----------
if(BML_ENABLE_WARNINGS)
    foreach(tgt IN ITEMS BML_static BML_shared)
        target_compile_options(${tgt} PRIVATE
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
                -Wall -Wextra -Wpedantic
                -Wno-c++98-compat -Wno-c++98-compat-pedantic
                -Wno-covered-switch-default -Wno-padded>
                $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        )
    endforeach()
endif()

# ---------- Per-config compile opts ----------
foreach(tgt IN ITEMS BML_static BML_shared)
    target_compile_options(${tgt} PRIVATE
            $<$<CONFIG:Debug>:-O0 -g>
            $<$<CONFIG:Release>:-O3 -DNDEBUG>
            $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
    )
endforeach()

# ---------- LTO (IPO) for libraries only ----------
if(BML_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_msg)
    if(_ipo_supported)
        # Enable IPO for both library targets in all configs
        set_property(TARGET BML_static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET BML_shared PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "IPO/LTO not supported: ${_ipo_msg}")
    endif()
endif()

# Prefer lld on Linux if available (only meaningful for shared lib link)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fuse-ld=lld" HAS_LLD_FLAG)
if(HAS_LLD_FLAG AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    target_link_options(BML_shared PRIVATE "-fuse-ld=lld")
endif()

# ---------- Test executable ----------
add_executable(testProgram ${BML_TEST_SOURCES})
target_link_libraries(testProgram PRIVATE BML_shared)
target_include_directories(testProgram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(testProgram PRIVATE
        $<$<CONFIG:Debug>:-O0 -g>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
)

# ---------- Install / package ----------
install(TARGETS BML_static BML_shared
        EXPORT BMLTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(TARGETS testProgram
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(BML_INSTALL_PACKAGE)
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/BMLConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion
    )
    configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/BMLConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/BMLConfig.cmake @ONLY)

    install(EXPORT BMLTargets
            NAMESPACE BML::
            FILE BMLTargets.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BML
    )
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/BMLConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/BMLConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BML
    )
endif()

# ---------- CTest ----------
include(CTest)
add_test(NAME bml_smoke COMMAND testProgram)
