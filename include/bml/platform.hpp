#ifndef BML_PLATFORM_HPP
#define BML_PLATFORM_HPP

#include <cstdint>

/**
 * @file bml/platform.hpp
 * @brief Platform / architecture requirements for BML.
 *
 * BML is only supported on 64-bit targets.
 *
 * The static_assert below intentionally fails the build on 32-bit (or god forbid 16-bit) systems
 * (where sizeof(void*) != 8). This prevents accidental builds on unsupported
 * architectures.
 *
 * Rationale:
 * - The library assumes 64-bit pointer width for some internal layouts,
 *   sizes, and index ranges.
 * - Behaviour on 32-bit systems is not tested and not guaranteed.
 *
 * Example:
 * On a 64-bit system (LP64 / LLP64), this compiles:
 * @code
 * static_assert(sizeof(void*) == 8);
 * @endcode
 *
 * On a 32-bit system, you'll get a compile-time error:
 * "BML requires a 64-bit target (pointer size must be 8 bytes)."
 */
static_assert(
    sizeof(void*) == 8,
    "BML requires a 64-bit target (pointer size must be 8 bytes)."
);

#endif // BML_PLATFORM_HPP
