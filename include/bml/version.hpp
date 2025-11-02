/**
* @file BMLVersion.hpp
 * @brief Compile-time version information for the BML library.
 *
 * These macros let you detect which version of BML you are compiling against.
 * They follow semantic versioning: MAJOR.MINOR.PATCH.
 *
 * Typical use:
 * @code
 * #include "BMLVersion.hpp"
 *
 * #if (BML_VERSION_MAJOR > 2) || \
 *     (BML_VERSION_MAJOR == 2 && BML_VERSION_MINOR >= 1)
 *     // code that requires at least BML 2.1.0
 * #endif
 * @endcode
 */

#ifndef BML_VERSION_HPP
#define BML_VERSION_HPP

/**
 * @def BML_VERSION_MAJOR
 * @brief Major version of the BML library.
 *
 * Increased when there are breaking API changes.
 */
#define BML_VERSION_MAJOR 2

/**
 * @def BML_VERSION_MINOR
 * @brief Minor version of the BML library.
 *
 * Increased when new features are added in a backwards-compatible way.
 */
#define BML_VERSION_MINOR 0

/**
 * @def BML_VERSION_PATCH
 * @brief Patch version of the BML library.
 *
 * Increased for bug fixes and other backwards-compatible changes.
 */
#define BML_VERSION_PATCH 0

#endif // BML_VERSION_HPP
