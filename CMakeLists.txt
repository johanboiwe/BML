cmake_minimum_required(VERSION 3.18)
project(BML VERSION 2.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()


# ---------- C++ standard ----------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ---------- Library sources ----------

set(BML_LIB_SOURCES
        src/instantiations.cpp
)


# ---------- Libraries ----------

add_library(BML_static STATIC ${BML_LIB_SOURCES})
add_library(BML_shared SHARED ${BML_LIB_SOURCES})

target_include_directories(BML_static
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(BML_shared
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(BML_TARGETS
        BML_static
        BML_shared
)


if(MSVC)
    # Deliberate design choices:
    #
    # C4804: bool relational operators (< and >) are provided for
    #        consistency with the other matrix element types.
    #
    # C4805: bool uses the byte-based storage mechanism rather than
    #        std::vector<bool>, avoiding its specialised packed representation.
    #
    # C4244: matrix dimensions are limited to uint32_t, while int64_t
    #        is used internally so that -1 can represent a sentinel value.

    target_compile_options(BML_shared PRIVATE
            /wd4804
            /wd4805
            /wd4244
    )

    target_compile_options(BML_static PRIVATE
            /wd4804
            /wd4805
            /wd4244
    )
endif()


# ---------- Default BML target ----------

option(BML_BUILD_SHARED "Build shared library" ON)

if(BML_BUILD_SHARED)
    add_library(BML ALIAS BML_shared)
else()
    add_library(BML ALIAS BML_static)
endif()


if(WIN32)
    target_compile_definitions(BML_shared
            PRIVATE
            BML_BUILDING_DLL
    )
endif()


# ---------- Tests ----------

enable_testing()

add_executable(testMatrix_shared
        src/testMatrix.cpp
)

target_link_libraries(testMatrix_shared
        PRIVATE
        BML_shared
)

add_executable(testMatrix_static
        src/testMatrix.cpp
)

target_link_libraries(testMatrix_static
        PRIVATE
        BML_static
)

add_test(
        NAME Matrix_Shared
        COMMAND testMatrix_shared
)

add_test(
        NAME Matrix_Static
        COMMAND testMatrix_static
)


# ---------- Library output names ----------

if(WIN32)
    set_target_properties(BML_static PROPERTIES
            OUTPUT_NAME BML_static
    )
else()
    set_target_properties(BML_static PROPERTIES
            OUTPUT_NAME BML
    )
endif()

set_target_properties(BML_shared PROPERTIES
        OUTPUT_NAME BML
)


# ---------- LTO ----------

include(CheckIPOSupported)

check_ipo_supported(
        RESULT BML_IPO_SUPPORTED
        OUTPUT BML_IPO_ERROR
        LANGUAGES CXX
)

if(BML_IPO_SUPPORTED)

    set_property(
            TARGET BML_static
            PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE
    )

    set_property(
            TARGET BML_shared
            PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE
    )

    set_property(
            TARGET testMatrix_shared
            PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE
    )

    set_property(
            TARGET testMatrix_static
            PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE
    )

    message(STATUS "Building with LTO")

else()

    message(WARNING
            "LTO/IPO is not supported: ${BML_IPO_ERROR}"
    )

endif()


# ---------- Installation ----------

install(TARGETS BML_static BML_shared
        EXPORT BMLTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BML
)


# ---------- CMake package ----------

install(EXPORT BMLTargets
        FILE BMLTargets.cmake
        NAMESPACE BML::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BML
)

configure_package_config_file(
        cmake/BMLConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/BMLConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BML
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/BMLConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/BMLConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/BMLConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BML
)


# ---------- Doxygen ----------

configure_file(
        Doxyfile
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
)