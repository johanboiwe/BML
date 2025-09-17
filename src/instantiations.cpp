// instantiations.cpp
// Central TU that pulls in template *definitions* by including the .cpps,
// and emits explicit instantiations for all supported types.

// Public headers (decls)
#include "bml.hpp"
#include "iterator.hpp"

// Pull in the template *definitions*.
// NOTE: Because we include these .cpp files here, you must NOT compile them
// separately in your build; otherwise you'll get duplicate symbols.
#include "bml.cpp"
#include "iterator.cpp"

// -------------------------------
// Local dtype lists (keep in sync)
// -------------------------------

// Math-enabled integrals (exclude plain char and bool)
#define BML_INTEGRAL_MATH_TYPES \
    X(short) X(int) X(long) X(long long) \
    X(unsigned short) X(unsigned int) X(unsigned long) X(unsigned long long) \
    X(signed char) X(unsigned char)

// Floating point
#define BML_FLOAT_TYPES \
    X(float) X(double) X(long double)

// Char-like (non-math)
#define BML_CHARLIKE_TYPES \
    X(char) X(wchar_t) X(char16_t) X(char32_t)

// Bool (non-math)
#define BML_BOOL_TYPES \
    X(bool)

// Specials
#define BML_SPECIAL_TYPES \
    X(std::string) X(void*)

// -----------------------------------
// Class template instantiations
// -----------------------------------

#define X(T) template class Matrix<T>;
BML_INTEGRAL_MATH_TYPES
BML_FLOAT_TYPES
BML_CHARLIKE_TYPES
BML_BOOL_TYPES
BML_SPECIAL_TYPES
#undef X

#define X(T) \
    template class Iterator<T>; \
    template class ConstIterator<T>;
BML_INTEGRAL_MATH_TYPES
BML_FLOAT_TYPES
BML_CHARLIKE_TYPES
BML_BOOL_TYPES
BML_SPECIAL_TYPES
#undef X

// -----------------------------------
// Member operator instantiations
// -----------------------------------

// + - * / (no %) helper
#define BML_INSTANTIATE_OPS_NO_MOD(T) \
    template Matrix<T> Matrix<T>::operator+(const Matrix<T>&) const; \
    template Matrix<T> Matrix<T>::operator-(const Matrix<T>&) const; \
    template Matrix<T> Matrix<T>::operator*(const Matrix<T>&) const; \
    template Matrix<T> Matrix<T>::operator/(const Matrix<T>&) const; \
    template Matrix<T> Matrix<T>::operator+(const T&) const;         \
    template Matrix<T> Matrix<T>::operator-(const T&) const;         \
    template Matrix<T> Matrix<T>::operator*(const T&) const;         \
    template Matrix<T> Matrix<T>::operator/(const T&) const;

// % helper (integrals only)
#define BML_INSTANTIATE_MOD(T) \
    template Matrix<T> Matrix<T>::operator%(const Matrix<T>&) const; \
    template Matrix<T> Matrix<T>::operator%(const T&) const;

// Math-enabled integrals: + - * / and %
#define X(T)  BML_INSTANTIATE_MOD(T) BML_INSTANTIATE_OPS_NO_MOD(T)
BML_INTEGRAL_MATH_TYPES
#undef X

// Floating point: + - * / only
#define X(T)  BML_INSTANTIATE_OPS_NO_MOD(T)
BML_FLOAT_TYPES
#undef X

#undef BML_INSTANTIATE_OPS_NO_MOD
#undef BML_INSTANTIATE_MOD
// (No arithmetic instantiations for char-like or bool)

// -----------------------------------
// Free comparison operator instantiations
// -----------------------------------

#define BML_INSTANTIATE_ALL_COMPARISONS(T) \
    template bool operator==<T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator!=<T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator< <T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator> <T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator<=<T>(const Matrix<T>&, const Matrix<T>&); \
    template bool operator>=<T>(const Matrix<T>&, const Matrix<T>&);

// Full set for non-pointer Ts (incl. bool, char-like, integrals, floats)
#define X(T) BML_INSTANTIATE_ALL_COMPARISONS(T)
BML_INTEGRAL_MATH_TYPES
BML_FLOAT_TYPES
BML_CHARLIKE_TYPES
BML_BOOL_TYPES
#undef X

// std::string — only eq/neq
template bool operator==<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
template bool operator!=<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);

// void* — only eq/neq (address equality)
template bool operator==<void*>(const Matrix<void*>&, const Matrix<void*>&);
template bool operator!=<void*>(const Matrix<void*>&, const Matrix<void*>&);

#undef BML_INSTANTIATE_ALL_COMPARISONS

// -------------------------------
// Undef local dtype-list macros
// -------------------------------
#undef BML_INTEGRAL_MATH_TYPES
#undef BML_FLOAT_TYPES
#undef BML_CHARLIKE_TYPES
#undef BML_BOOL_TYPES
#undef BML_SPECIAL_TYPES

