cmake_minimum_required(VERSION 3.18)
project(BML VERSION 2.0.0 LANGUAGES CXX)
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

set(BML_TARGETS
        BML_static
        BML_shared
)


# ---------- Default BML target ----------

option(BML_BUILD_SHARED "Build shared library" ON)

if(BML_BUILD_SHARED)
    add_library(BML ALIAS BML_shared)
else()
    add_library(BML ALIAS BML_static)
endif()


# ---------- Tests ----------

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


# ---------- Automatic tests ----------

option(BML_RUN_TESTS "Run tests as part of the build" ON)

enable_testing()


if(BML_RUN_TESTS)

    message(STATUS "Automatic tests: ON")

    # Shared library test
    add_custom_target(test_shared ALL
            COMMAND ${CMAKE_COMMAND} -E echo "===== Testing shared library ====="
            COMMAND $<TARGET_FILE:testMatrix_shared>
            DEPENDS testMatrix_shared
            USES_TERMINAL
    )

    # Static library test
    add_custom_target(test_static ALL
            COMMAND ${CMAKE_COMMAND} -E echo "===== Testing static library ====="
            COMMAND $<TARGET_FILE:testMatrix_static>
            DEPENDS testMatrix_static
            USES_TERMINAL
    )

else()

    message(STATUS "Automatic tests: OFF")

endif()

include(GNUInstallDirs)

install(TARGETS BML_static BML_shared
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BML
)
