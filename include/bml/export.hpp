#ifndef BML_EXPORT_HPP
#define BML_EXPORT_HPP

/**
 * @file bml/export.hpp
 * @brief Symbol visibility / import-export control for BML.
 *
 * This header defines two macros used to control which symbols are visible
 * outside the library when building or using BML as a shared library.
 *
 * - `BML_API` marks a symbol as part of the public ABI.
 *   - On Windows:
 *     - When building the DLL, `BML_BUILDING_DLL` should be defined, so
 *       `BML_API` becomes `__declspec(dllexport)`.
 *     - When using the DLL from an application, `BML_USE_DLL` should be
 *       defined, so `BML_API` becomes `__declspec(dllimport)`.
 *     - When building/using BML as a static library, neither macro is set,
 *       so `BML_API` is empty.
 *
 *   - On GCC/Clang (ELF platforms like Linux, BSD, etc.):
 *     - If `BML_STATIC` is *not* defined, `BML_API` expands to
 *       `__attribute__((visibility("default")))`, which makes the symbol
 *       externally visible from the shared object.
 *     - If `BML_STATIC` *is* defined (building/using a static lib),
 *       `BML_API` is empty.
 *
 * - `BML_HIDDEN` marks a symbol as internal / not exported from the shared lib.
 *   This is only meaningful on GCC/Clang. On other compilers it currently
 *   expands to nothing.
 *
 * Typical usage in a header that declares something public:
 * @code
 * #include "BMLExport.hpp"
 *
 * class BML_API MatrixBase {
 * public:
 *     virtual ~MatrixBase() = default;
 *     // ...
 * };
 *
 * BML_API void initialiseBML();
 * @endcode
 *
 * You normally do NOT define these macros yourself in user code. They are set
 * by the build system (CMake) when building or consuming BML.
 */

#if defined(_WIN32)

  #if defined(BML_BUILDING_DLL)
    /// Exporting symbols while building the BML DLL (Windows).
    #define BML_API __declspec(dllexport)

  #elif defined(BML_USE_DLL)
    /// Importing symbols while consuming the BML DLL (Windows).
    #define BML_API __declspec(dllimport)

  #else
    /// Static build or no explicit import/export needed (Windows).
    #define BML_API
  #endif

  /// Hidden symbols are not specifically controlled on Windows here.
  #define BML_HIDDEN

#else // non-Windows

  #if defined(__GNUC__) || defined(__clang__)

    #if !defined(BML_STATIC)
      /// Publicly visible symbol in a shared object (ELF platforms).
      #define BML_API __attribute__((visibility("default")))
    #else
      /// Static build on ELF platforms: no visibility attribute needed.
      #define BML_API
    #endif

    /// Internal/non-exported symbol in a shared object (ELF platforms).
    #define BML_HIDDEN __attribute__((visibility("hidden")))

  #else
    /// Fallback: no special attributes known for this compiler.
    #define BML_API
    #define BML_HIDDEN
  #endif

#endif

#endif // BML_EXPORT_HPP
