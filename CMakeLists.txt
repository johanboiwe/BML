cmake_minimum_required(VERSION 3.20)
project(BML VERSION 2.0.0 LANGUAGES CXX)

# ---------- C++ standard ----------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------- Options ----------
option(BML_ENABLE_WARNINGS     "Enable extra warnings" ON)
option(BML_INSTALL_PACKAGE     "Install CMake package config" ON)

# LTO via flags; Debug can opt-in.
option(BML_ENABLE_LTO          "Enable LTO on Release/RelWithDebInfo" ON)
option(BML_LTO_ON_DEBUG        "Also enable LTO on Debug" OFF)

# Choose flavour per compiler (can be overridden on the CLI)
# Clang/AppleClang
set(BML_LTO_FLAG_CLANG       "-flto=full"          CACHE STRING "Clang LTO flag (full/thin)")
set(BML_LTO_FLAG_APPLECLANG  "-flto=thin"          CACHE STRING "AppleClang LTO flag (thin/full)")
set(BML_LTO_LINK_CLANG_DBG   "-Wl,--lto-O0"        CACHE STRING "LLD LTO level for Debug (Clang+LLD)")
set(BML_LTO_LINK_CLANG_REL   "-Wl,--lto-O3"        CACHE STRING "LLD LTO level for Release (Clang+LLD)")
# GCC
set(BML_LTO_FLAG_GNU         "-flto"               CACHE STRING "GCC LTO flag")
# MSVC
set(BML_LTO_FLAG_MSVC        "/GL"                 CACHE STRING "MSVC compile-time LTCG flag")
set(BML_LTO_LINK_MSVC        "/LTCG"               CACHE STRING "MSVC link-time LTCG flag")

# ---------- Default build type for single-config generators ----------
set(DEFAULT_BUILD_TYPE "RelWithDebInfo")
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Build type" FORCE)
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

include(GNUInstallDirs)

# ---------- Sources ----------
set(BML_LIB_SOURCES
        src/instantiations.cpp
        src/stringStorage.cpp
)
set(BML_TEST_SOURCES
        src/main.cpp
        src/testMatrix.cpp
)

# ---------- Libraries ----------
add_library(BML_static STATIC ${BML_LIB_SOURCES})
add_library(BML_shared SHARED ${BML_LIB_SOURCES})
set(BML_TARGETS BML_static BML_shared)

# Names / versions
set_target_properties(${BML_TARGETS} PROPERTIES OUTPUT_NAME BML)
set_target_properties(BML_shared PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

# Public includes
target_include_directories(BML_static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(BML_shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Tell headers we are building the lib
target_compile_definitions(BML_static PRIVATE BML_BUILDING=1)
target_compile_definitions(BML_shared PRIVATE BML_BUILDING=1)

# ---------- Warnings per compiler ----------
if(BML_ENABLE_WARNINGS)
    foreach(tgt IN LISTS BML_TARGETS)
        target_compile_options(${tgt} PRIVATE
                # GCC
                $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
                # Clang/AppleClang
                $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wall -Wextra -Wpedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-covered-switch-default -Wno-padded>
                # MSVC
                $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        )
    endforeach()
endif()

# ---------- Per-config additions (optimisation flags are provided by CMake) ----------
foreach(tgt IN LISTS BML_TARGETS)
    target_compile_options(${tgt} PRIVATE
            $<$<CONFIG:Debug>:-g>
            $<$<AND:$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>,$<CONFIG:Debug>>:-fno-omit-frame-pointer>
            $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:-fno-omit-frame-pointer>
            $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi /Ob0 /Od>
    )
endforeach()

# ---------- LTO flags per compiler (no IPO variable flipping) ----------
if(BML_ENABLE_LTO)
    foreach(tgt IN LISTS BML_TARGETS)
        # Release / RelWithDebInfo
        target_compile_options(${tgt} PRIVATE
                # GCC
                $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Release>>:${BML_LTO_FLAG_GNU}>
                $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RelWithDebInfo>>:${BML_LTO_FLAG_GNU}>
                # Clang
                $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:${BML_LTO_FLAG_CLANG}>
                $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:RelWithDebInfo>>:${BML_LTO_FLAG_CLANG}>
                # AppleClang
                $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<CONFIG:Release>>:${BML_LTO_FLAG_APPLECLANG}>
                $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<CONFIG:RelWithDebInfo>>:${BML_LTO_FLAG_APPLECLANG}>
                # MSVC
                $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:${BML_LTO_FLAG_MSVC}>
                $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:${BML_LTO_FLAG_MSVC}>
        )
        # Debug opt-in
        target_compile_options(${tgt} PRIVATE
                $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${BML_LTO_ON_DEBUG}>,$<CONFIG:Debug>>:${BML_LTO_FLAG_GNU}>
                $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${BML_LTO_ON_DEBUG}>,$<CONFIG:Debug>>:${BML_LTO_FLAG_CLANG}>
                $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<BOOL:${BML_LTO_ON_DEBUG}>,$<CONFIG:Debug>>:${BML_LTO_FLAG_APPLECLANG}>
                $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<BOOL:${BML_LTO_ON_DEBUG}>,$<CONFIG:Debug>>:${BML_LTO_FLAG_MSVC}>
        )
    endforeach()

    # Link flags for shared lib
    target_link_options(BML_shared PRIVATE
            # GCC
            $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Release>>:${BML_LTO_FLAG_GNU}>
            $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RelWithDebInfo>>:${BML_LTO_FLAG_GNU}>
            $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${BML_LTO_ON_DEBUG}>,$<CONFIG:Debug>>:${BML_LTO_FLAG_GNU}>

            # Clang + LLD (if used): add --lto-O*; safe to omit on Apple ld
            $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:${BML_LTO_FLAG_CLANG} ${BML_LTO_LINK_CLANG_REL}>
            $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:RelWithDebInfo>>:${BML_LTO_FLAG_CLANG} ${BML_LTO_LINK_CLANG_REL}>
            $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${BML_LTO_ON_DEBUG}>,$<CONFIG:Debug>>:${BML_LTO_FLAG_CLANG} ${BML_LTO_LINK_CLANG_DBG}>

            # AppleClang
            $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<CONFIG:Release>>:${BML_LTO_FLAG_APPLECLANG}>
            $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<CONFIG:RelWithDebInfo>>:${BML_LTO_FLAG_APPLECLANG}>
            $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<BOOL:${BML_LTO_ON_DEBUG}>,$<CONFIG:Debug>>:${BML_LTO_FLAG_APPLECLANG}>

            # MSVC
            $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:${BML_LTO_LINK_MSVC}>
            $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:${BML_LTO_LINK_MSVC}>
            $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<BOOL:${BML_LTO_ON_DEBUG}>,$<CONFIG:Debug>>:${BML_LTO_LINK_MSVC}>
    )
endif()

# ---------- Prefer lld on Linux for shared link (Clang/GCC only) ----------
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fuse-ld=lld" HAS_LLD_FLAG)
if(HAS_LLD_FLAG AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    target_link_options(BML_shared PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fuse-ld=lld>
    )
endif()

# ---------- Test executable ----------
add_executable(testProgram ${BML_TEST_SOURCES})
target_link_libraries(testProgram PRIVATE BML_shared)
target_include_directories(testProgram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---------- Install / package ----------
# Install both libs with export
install(TARGETS BML_shared BML_static
        EXPORT BMLTargets
        ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}   # Windows DLLs
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Package config
if(BML_INSTALL_PACKAGE)
    include(CMakePackageConfigHelpers)
    configure_package_config_file(
            ${CMAKE_CURRENT_LIST_DIR}/cmake/BMLConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/BMLConfig.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BML
    )
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/BMLConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion
    )
    install(EXPORT BMLTargets NAMESPACE BML:: FILE BMLTargets.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BML)
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/BMLConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/BMLConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BML)
endif()

# ---------- Tests: add & run on every build ----------
include(CTest)
add_test(NAME bml_smoke COMMAND testProgram)

# Build both libs explicitly and always run tests
add_custom_target(bml_all ALL
        DEPENDS BML_shared BML_static
)
add_custom_target(run_tests ALL
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<IF:$<BOOL:${CMAKE_CONFIGURATION_TYPES}>,$<CONFIG>,${CMAKE_BUILD_TYPE}>
        DEPENDS testProgram
        USES_TERMINAL
)
