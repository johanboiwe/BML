// instantiations.cpp
// Single TU that pulls in template *definitions* and emits explicit instantiations.
// IMPORTANT: Do NOT compile bml.cpp / iterator.cpp / rowView.cpp separately.

#include <cstdint>
#include <string>

#include "bml.hpp"
#include "iterator.hpp"
#include "rowView.hpp"

// Pull in template *definitions* so explicit instantiations link from here.
#include "bml.cpp"
#include "iterator.cpp"
#include "rowView.cpp"

// -------------------------------
// Fixed-width type lists
// -------------------------------

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

// If you don’t want these char-like types, remove them from here.
#define BML_CHARLIKE_TYPES(X) \
    X(char)

#define BML_BOOL_TYPES(X) \
    X(bool)

#define BML_SPECIAL_TYPES(X) \
    X(std::string)

// -----------------------------------
// Class template instantiations
// -----------------------------------

#define X(T) template class Matrix<T>;
BML_INTEGRAL_MATH_TYPES(X)
BML_FLOAT_TYPES(X)
BML_CHARLIKE_TYPES(X)
BML_BOOL_TYPES(X)
BML_SPECIAL_TYPES(X)
#undef X

#define X(T) template class Iterator<T>; template class ConstIterator<T>;
BML_INTEGRAL_MATH_TYPES(X)
BML_FLOAT_TYPES(X)
BML_CHARLIKE_TYPES(X)
BML_BOOL_TYPES(X)
BML_SPECIAL_TYPES(X)
#undef X

// If RowView<bool> is a distinct specialisation you provide elsewhere, leave it out here.
#define X(T) template class RowView<T>; template class RowView<const T>;
BML_INTEGRAL_MATH_TYPES(X)
BML_FLOAT_TYPES(X)
BML_CHARLIKE_TYPES(X)
BML_SPECIAL_TYPES(X)
#undef X

// -----------------------------------
// Member function template instantiations (U = T)
// -----------------------------------

// Reductions (sum/min/max/argmin/argmax)
#define INSTANTIATE_REDUCTIONS(T) \
    template T Matrix<T>::sum<T>() const; \
    template T Matrix<T>::min<T>() const; \
    template T Matrix<T>::max<T>() const; \
    template std::pair<std::uint32_t,std::uint32_t> Matrix<T>::argmin<T>() const; \
    template std::pair<std::uint32_t,std::uint32_t> Matrix<T>::argmax<T>() const;

// Arithmetic (+ - * /), both matrix⊕matrix and matrix⊕scalar, plus compound forms
#define INSTANTIATE_ARITH(T) \
    /* matrix ⊕ matrix */ \
    template Matrix<T> Matrix<T>::operator+<T>(const Matrix<T>&) const; \
    template Matrix<T> Matrix<T>::operator-<T>(const Matrix<T>&) const; \
    template Matrix<T> Matrix<T>::operator*<T>(const Matrix<T>&) const; \
    template Matrix<T> Matrix<T>::operator/<T>(const Matrix<T>&) const; \
    /* matrix ⊕ scalar */ \
    template Matrix<T> Matrix<T>::operator+<T>(const T&) const; \
    template Matrix<T> Matrix<T>::operator-<T>(const T&) const; \
    template Matrix<T> Matrix<T>::operator*<T>(const T&) const; \
    template Matrix<T> Matrix<T>::operator/<T>(const T&) const; \
    /* compound matrix ⊕= matrix */ \
    template Matrix<T>& Matrix<T>::operator+=<T>(const Matrix<T>&); \
    template Matrix<T>& Matrix<T>::operator-=<T>(const Matrix<T>&); \
    template Matrix<T>& Matrix<T>::operator*=<T>(const Matrix<T>&); \
    template Matrix<T>& Matrix<T>::operator/=<T>(const Matrix<T>&); \
    /* compound matrix ⊕= scalar */ \
    template Matrix<T>& Matrix<T>::operator+=<T>(const T&); \
    template Matrix<T>& Matrix<T>::operator-=<T>(const T&); \
    template Matrix<T>& Matrix<T>::operator*=<T>(const T&); \
    template Matrix<T>& Matrix<T>::operator/=<T>(const T&);

// Modulus + bitwise + shifts (integral “real” types only — your trait excludes plain char & bool)
#define INSTANTIATE_INT_ONLY(T) \
    /* % (matrix/matrix and matrix/scalar) */ \
    template Matrix<T>  Matrix<T>::operator%<T>(const Matrix<T>&) const; \
    template Matrix<T>  Matrix<T>::operator%<T>(const T&) const; \
    template Matrix<T>& Matrix<T>::operator%=<T>(const Matrix<T>&); \
    template Matrix<T>& Matrix<T>::operator%=<T>(const T&); \
    /* &, |, ^ (matrix and scalar) */ \
    template Matrix<T>  Matrix<T>::operator&<T>(const Matrix<T>&) const; \
    template Matrix<T>  Matrix<T>::operator|<T>(const Matrix<T>&) const; \
    template Matrix<T>  Matrix<T>::operator^<T>(const Matrix<T>&) const; \
    template Matrix<T>  Matrix<T>::operator&<T>(const T&) const; \
    template Matrix<T>  Matrix<T>::operator|<T>(const T&) const; \
    template Matrix<T>  Matrix<T>::operator^<T>(const T&) const; \
    /* compound bitwise */ \
    template Matrix<T>& Matrix<T>::operator&=<T>(const Matrix<T>&); \
    template Matrix<T>& Matrix<T>::operator|=<T>(const Matrix<T>&); \
    template Matrix<T>& Matrix<T>::operator^=<T>(const Matrix<T>&); \
    template Matrix<T>& Matrix<T>::operator&=<T>(const T&); \
    template Matrix<T>& Matrix<T>::operator|=<T>(const T&); \
    template Matrix<T>& Matrix<T>::operator^=<T>(const T&); \
    /* unary ~ and shifts */ \
    template Matrix<T>  Matrix<T>::operator~<T>() const; \
    template Matrix<T>  Matrix<T>::operator<< <T>(int) const; \
    template Matrix<T>  Matrix<T>::operator>> <T>(int) const; \
    template Matrix<T>& Matrix<T>::operator<<=<T>(int); \
    template Matrix<T>& Matrix<T>::operator>>=<T>(int);

// Bool logical API (matrix/matrix, matrix/scalar, and reductions)
#define INSTANTIATE_BOOL_SUITE() \
    template Matrix<bool> Matrix<bool>::logical_and<bool>(const Matrix<bool>&) const; \
    template Matrix<bool> Matrix<bool>::logical_or <bool>(const Matrix<bool>&) const; \
    template Matrix<bool> Matrix<bool>::logical_xor<bool>(const Matrix<bool>&) const; \
    template Matrix<bool> Matrix<bool>::logical_not<bool>() const; \
    template Matrix<bool> Matrix<bool>::logical_and<bool>(bool) const; \
    template Matrix<bool> Matrix<bool>::logical_or <bool>(bool) const; \
    template Matrix<bool> Matrix<bool>::logical_xor<bool>(bool) const; \
    template std::size_t Matrix<bool>::count_true<bool>() const noexcept; \
    template bool        Matrix<bool>::any<bool>() const noexcept; \
    template bool        Matrix<bool>::none<bool>() const noexcept;
    // ---- Bool reductions used in tests ----
template bool Matrix<bool>::min<bool>() const;
template bool Matrix<bool>::max<bool>() const;

// (Optional but recommended for consistency)
template std::pair<std::uint32_t,std::uint32_t> Matrix<bool>::argmin<bool>() const;
template std::pair<std::uint32_t,std::uint32_t> Matrix<bool>::argmax<bool>() const;

// ---- Apply sets ----

// Arithmetic + reductions for arithmetic types (your trait excludes plain char & bool)
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

// Integral-only ops (%, bitwise, shifts) — excludes plain char & bool via your trait
INSTANTIATE_INT_ONLY(std::int8_t)
INSTANTIATE_INT_ONLY(std::uint8_t)
INSTANTIATE_INT_ONLY(std::int16_t)
INSTANTIATE_INT_ONLY(std::uint16_t)
INSTANTIATE_INT_ONLY(std::int32_t)
INSTANTIATE_INT_ONLY(std::uint32_t)
INSTANTIATE_INT_ONLY(std::int64_t)
INSTANTIATE_INT_ONLY(std::uint64_t)

// Bool suite
INSTANTIATE_BOOL_SUITE()

// Strings: special I/O (definitions must exist in the included bml.cpp)
//template void                 Matrix<std::string>::initFromByteStream(const uint8_t*, size_t);
//template std::vector<uint8_t> Matrix<std::string>::toByteStream() const;

// -----------------------------------
// Free comparison operator instantiations
// -----------------------------------
#define BML_INSTANTIATE_COMPARISONS(T) \
    template bool operator==<T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator!=<T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator< <T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator> <T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator<=<T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator>=<T>(const Matrix<T>&, const Matrix<T>&);

#define X(T) BML_INSTANTIATE_COMPARISONS(T)
BML_INTEGRAL_MATH_TYPES(X)
BML_FLOAT_TYPES(X)
BML_CHARLIKE_TYPES(X)
BML_BOOL_TYPES(X)
#undef X

// For std::string: only equality/inequality are meaningful here
template bool operator==<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
template bool operator!=<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
static_assert(std::is_same_v<decltype(std::declval<Matrix<int>&>()[0][0]), int&>);
static_assert(std::is_same_v<decltype(std::declval<Matrix<bool>&>()[0][0]), BoolRef>);
static_assert(std::is_same_v<decltype(std::declval<const Matrix<int>&>()[0][0]), const int&>);
static_assert(std::is_same_v<decltype(std::declval<const Matrix<bool>&>()[0][0]), bool>);

// -------------------------------
// Undefine macros
// -------------------------------
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
