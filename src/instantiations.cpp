// instantiations.cpp
// Single TU that pulls in template *definitions* and emits explicit instantiations.
// IMPORTANT: Do NOT compile bml.cpp / iterator.cpp / rowView.cpp separately.

#include <cstdint>
#include <cstddef>
#include <string>
#include <utility>
#include <type_traits>

#include "bml.hpp"
#include "iterator.hpp"
#include "rowView.hpp"

// -----------------------------------------------------------------------------
// Visibility/export (no pragmas; symbol export is via BML_API on instantiations)
// -----------------------------------------------------------------------------
#if !defined(BML_API)
  #if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(BML_BUILD_DLL)
      #define BML_API __declspec(dllexport)
    #else
      #define BML_API __declspec(dllimport)
    #endif
  #else
    #define BML_API __attribute__((visibility("default")))
  #endif
#endif

// -----------------------------------------------------------------------------
// Pull in template *definitions* so explicit instantiations link from here.
// (These .cpps must contain the full template definitions used by the API.)
#include "bml.cpp"
#include "iterator.cpp"
#include "rowView.cpp"

// -----------------------------------------------------------------------------
// Type lists
// -----------------------------------------------------------------------------
#define BML_STDINT_SIGNED_TYPES(X)   \
    X(std::int8_t)                   \
    X(std::int16_t)                  \
    X(std::int32_t)                  \
    X(std::int64_t)

#define BML_STDINT_UNSIGNED_TYPES(X) \
    X(std::uint8_t)                  \
    X(std::uint16_t)                 \
    X(std::uint32_t)                 \
    X(std::uint64_t)

#define BML_INTEGRAL_MATH_TYPES(X)   \
    BML_STDINT_SIGNED_TYPES(X)       \
    BML_STDINT_UNSIGNED_TYPES(X)

#define BML_FLOAT_TYPES(X) \
    X(float)               \
    X(double)              \
    X(long double)

#define BML_CHARLIKE_TYPES(X) \
    X(char)

#define BML_BOOL_TYPES(X) \
    X(bool)

#define BML_SPECIAL_TYPES(X) \
    X(std::string)

// -----------------------------------------------------------------------------
// Class template instantiations (INCLUDING BOOL)
// -----------------------------------------------------------------------------

// Matrix
#define X(T) template class BML_API Matrix<T>;
BML_INTEGRAL_MATH_TYPES(X)
BML_FLOAT_TYPES(X)
BML_CHARLIKE_TYPES(X)
BML_BOOL_TYPES(X)
BML_SPECIAL_TYPES(X)
#undef X

// Iterator / ConstIterator
#define X(T) template class BML_API Iterator<T>; template class BML_API ConstIterator<T>;
BML_INTEGRAL_MATH_TYPES(X)
BML_FLOAT_TYPES(X)
BML_CHARLIKE_TYPES(X)
BML_BOOL_TYPES(X)
BML_SPECIAL_TYPES(X)
#undef X

// RowView (+ const)
// NOTE: if rowView.hpp defines explicit specialisations for bool,
// explicitly instantiate those specialisations directly to avoid
// -Winstantiation-after-specialization.
#define X(T) template class BML_API RowView<T>; template class BML_API RowView<const T>;
BML_INTEGRAL_MATH_TYPES(X)
BML_FLOAT_TYPES(X)
BML_CHARLIKE_TYPES(X)
// no BML_BOOL_TYPES(X) here (bool handled below)
BML_SPECIAL_TYPES(X)
#undef X



// -----------------------------------------------------------------------------
// Member function template instantiations (U = T)
// Only needed if these are out-of-line in the included .cpps.
// -----------------------------------------------------------------------------

// Reductions (sum/min/max/argmin/argmax)
#define INSTANTIATE_REDUCTIONS(T) \
    template BML_API T Matrix<T>::sum<T>() const; \
    template BML_API T Matrix<T>::min<T>() const; \
    template BML_API T Matrix<T>::max<T>() const; \
    template BML_API std::pair<std::uint32_t,std::uint32_t> Matrix<T>::argmin<T>() const; \
    template BML_API std::pair<std::uint32_t,std::uint32_t> Matrix<T>::argmax<T>() const;

// Arithmetic (+ - * /), both matrix⊕matrix and matrix⊕scalar, plus compound forms
#define INSTANTIATE_ARITH(T) \
    /* matrix ⊕ matrix */ \
    template BML_API Matrix<T>  Matrix<T>::operator+<T>(const Matrix<T>&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator-<T>(const Matrix<T>&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator*<T>(const Matrix<T>&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator/<T>(const Matrix<T>&) const; \
    /* matrix ⊕ scalar */ \
    template BML_API Matrix<T>  Matrix<T>::operator+<T>(const T&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator-<T>(const T&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator*<T>(const T&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator/<T>(const T&) const; \
    /* compound matrix ⊕= matrix */ \
    template BML_API Matrix<T>& Matrix<T>::operator+=<T>(const Matrix<T>&); \
    template BML_API Matrix<T>& Matrix<T>::operator-=<T>(const Matrix<T>&); \
    template BML_API Matrix<T>& Matrix<T>::operator*=<T>(const Matrix<T>&); \
    template BML_API Matrix<T>& Matrix<T>::operator/=<T>(const Matrix<T>&); \
    /* compound matrix ⊕= scalar */ \
    template BML_API Matrix<T>& Matrix<T>::operator+=<T>(const T&); \
    template BML_API Matrix<T>& Matrix<T>::operator-=<T>(const T&); \
    template BML_API Matrix<T>& Matrix<T>::operator*=<T>(const T&); \
    template BML_API Matrix<T>& Matrix<T>::operator/=<T>(const T&);

// Modulus + bitwise + shifts (integral “real” types only — excludes plain char & bool)
#define INSTANTIATE_INT_ONLY(T) \
    /* % (matrix/matrix and matrix/scalar) */ \
    template BML_API Matrix<T>  Matrix<T>::operator%<T>(const Matrix<T>&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator%<T>(const T&) const; \
    template BML_API Matrix<T>& Matrix<T>::operator%=<T>(const Matrix<T>&); \
    template BML_API Matrix<T>& Matrix<T>::operator%=<T>(const T&); \
    /* &, |, ^ (matrix and scalar) */ \
    template BML_API Matrix<T>  Matrix<T>::operator&<T>(const Matrix<T>&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator|<T>(const Matrix<T>&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator^<T>(const Matrix<T>&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator&<T>(const T&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator|<T>(const T&) const; \
    template BML_API Matrix<T>  Matrix<T>::operator^<T>(const T&) const; \
    /* compound bitwise (note: SFINAE in return type must match your declarations) */ \
    template BML_API typename std::enable_if<bml_is_math_integral<T>::value, Matrix<T>&>::type \
      Matrix<T>::operator&=<T>(const Matrix<T>&); \
    template BML_API typename std::enable_if<bml_is_math_integral<T>::value, Matrix<T>&>::type \
      Matrix<T>::operator|=<T>(const Matrix<T>&); \
    template BML_API typename std::enable_if<bml_is_math_integral<T>::value, Matrix<T>&>::type \
      Matrix<T>::operator^=<T>(const Matrix<T>&); \
    template BML_API typename std::enable_if<bml_is_math_integral<T>::value, Matrix<T>&>::type \
      Matrix<T>::operator&=<T>(const T&); \
    template BML_API typename std::enable_if<bml_is_math_integral<T>::value, Matrix<T>&>::type \
      Matrix<T>::operator|=<T>(const T&); \
    template BML_API typename std::enable_if<bml_is_math_integral<T>::value, Matrix<T>&>::type \
      Matrix<T>::operator^=<T>(const T&); \
    /* unary ~ and shifts */ \
    template BML_API Matrix<T>  Matrix<T>::operator~<T>() const; \
    template BML_API Matrix<T>  Matrix<T>::operator<< <T>(int) const; \
    template BML_API Matrix<T>  Matrix<T>::operator>> <T>(int) const; \
    template BML_API Matrix<T>& Matrix<T>::operator<<=<T>(int); \
    template BML_API Matrix<T>& Matrix<T>::operator>>=<T>(int);

// Bool logical API (matrix/matrix, matrix/scalar, and reductions)
#define INSTANTIATE_BOOL_SUITE() \
    template BML_API Matrix<bool> Matrix<bool>::logical_and<bool>(const Matrix<bool>&) const; \
    template BML_API Matrix<bool> Matrix<bool>::logical_or <bool>(const Matrix<bool>&) const; \
    template BML_API Matrix<bool> Matrix<bool>::logical_xor<bool>(const Matrix<bool>&) const; \
    template BML_API Matrix<bool> Matrix<bool>::logical_not<bool>() const; \
    template BML_API Matrix<bool> Matrix<bool>::logical_and<bool>(bool) const; \
    template BML_API Matrix<bool> Matrix<bool>::logical_or <bool>(bool) const; \
    template BML_API Matrix<bool> Matrix<bool>::logical_xor<bool>(bool) const; \
    template BML_API std::size_t  Matrix<bool>::count_true<bool>() const noexcept; \
    template BML_API bool         Matrix<bool>::any<bool>() const noexcept; \
    template BML_API bool         Matrix<bool>::none<bool>() const noexcept; \
    template BML_API bool         Matrix<bool>::min<bool>() const; \
    template BML_API bool         Matrix<bool>::max<bool>() const; \
    template BML_API std::pair<std::uint32_t,std::uint32_t> Matrix<bool>::argmin<bool>() const; \
    template BML_API std::pair<std::uint32_t,std::uint32_t> Matrix<bool>::argmax<bool>() const;

// ---- Apply sets ----
INSTANTIATE_ARITH(std::int8_t)    INSTANTIATE_REDUCTIONS(std::int8_t)
INSTANTIATE_ARITH(std::uint8_t)   INSTANTIATE_REDUCTIONS(std::uint8_t)
INSTANTIATE_ARITH(std::int16_t)   INSTANTIATE_REDUCTIONS(std::int16_t)
INSTANTIATE_ARITH(std::uint16_t)  INSTANTIATE_REDUCTIONS(std::uint16_t)
INSTANTIATE_ARITH(std::int32_t)   INSTANTIATE_REDUCTIONS(std::int32_t)
INSTANTIATE_ARITH(std::uint32_t)  INSTANTIATE_REDUCTIONS(std::uint32_t)
INSTANTIATE_ARITH(std::int64_t)   INSTANTIATE_REDUCTIONS(std::int64_t)
INSTANTIATE_ARITH(std::uint64_t)  INSTANTIATE_REDUCTIONS(std::uint64_t)
INSTANTIATE_ARITH(float)          INSTANTIATE_REDUCTIONS(float)
INSTANTIATE_ARITH(double)         INSTANTIATE_REDUCTIONS(double)
INSTANTIATE_ARITH(long double)    INSTANTIATE_REDUCTIONS(long double)

INSTANTIATE_INT_ONLY(std::int8_t)
INSTANTIATE_INT_ONLY(std::uint8_t)
INSTANTIATE_INT_ONLY(std::int16_t)
INSTANTIATE_INT_ONLY(std::uint16_t)
INSTANTIATE_INT_ONLY(std::int32_t)
INSTANTIATE_INT_ONLY(std::uint32_t)
INSTANTIATE_INT_ONLY(std::int64_t)
INSTANTIATE_INT_ONLY(std::uint64_t)

INSTANTIATE_BOOL_SUITE()

// Strings: add if you have out-of-line defs in bml.cpp
// template BML_API void                 Matrix<std::string>::initFromByteStream(const uint8_t*, size_t);
// template BML_API std::vector<uint8_t> Matrix<std::string>::toByteStream() const;

// -----------------------------------------------------------------------------
// Free comparison operator instantiations
// -----------------------------------------------------------------------------
#define BML_INSTANTIATE_COMPARISONS(T) \
    template BML_API bool operator==<T>(const Matrix<T>&, const Matrix<T>&); \
    template BML_API bool operator!=<T>(const Matrix<T>&, const Matrix<T>&); \
    template BML_API bool operator< <T>(const Matrix<T>&, const Matrix<T>&); \
    template BML_API bool operator> <T>(const Matrix<T>&, const Matrix<T>&); \
    template BML_API bool operator<=<T>(const Matrix<T>&, const Matrix<T>&); \
    template BML_API bool operator>=<T>(const Matrix<T>&, const Matrix<T>&);

#define X(T) BML_INSTANTIATE_COMPARISONS(T)
BML_INTEGRAL_MATH_TYPES(X)
BML_FLOAT_TYPES(X)
BML_CHARLIKE_TYPES(X)
BML_BOOL_TYPES(X)
#undef X

// For std::string: only equality/inequality are meaningful here
template BML_API bool operator==<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
template BML_API bool operator!=<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);

// -----------------------------------------------------------------------------
// Sanity checks for operator[] result types (requires BoolRef from rowView.hpp)
// -----------------------------------------------------------------------------
static_assert(std::is_same_v<decltype(std::declval<Matrix<int>&>()[0][0]), int&>);
static_assert(std::is_same_v<decltype(std::declval<Matrix<bool>&>()[0][0]), BoolRef>);
static_assert(std::is_same_v<decltype(std::declval<const Matrix<int>&>()[0][0]), const int&>);
static_assert(std::is_same_v<decltype(std::declval<const Matrix<bool>&>()[0][0]), bool>);

// -----------------------------------------------------------------------------
// Cleanup
// -----------------------------------------------------------------------------
#undef INSTANTIATE_REDUCTIONS
#undef INSTANTIATE_ARITH
#undef INSTANTIATE_INT_ONLY
#undef INSTANTIATE_BOOL_SUITE

#undef BML_STDINT_SIGNED_TYPES
#undef BML_STDINT_UNSIGNED_TYPES
#undef BML_INTEGRAL_MATH_TYPES
#undef BML_FLOAT_TYPES
#undef BML_CHARLIKE_TYPES
#undef BML_BOOL_TYPES
#undef BML_SPECIAL_TYPES
#undef BML_INSTANTIATE_COMPARISONS
