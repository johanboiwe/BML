#ifndef PLATFORM_HPP
#define PLATFORM_HPP
#include <cstdint>

// Portable, compile-time check (no compiler-specific macros needed)
static_assert(sizeof(void*) == 8, "BML requires a 64-bit target (pointer size must be 8 bytes).");
#define BML_PLATFORM_HPP

#endif // PLATFORM_HPP
