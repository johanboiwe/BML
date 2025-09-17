#ifndef BML_HPP_INCLUDED
#define BML_HPP_INCLUDED

#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <functional>
#include <type_traits>
#include "iterator.hpp"

// ---- helper traits to refine which Ts allow maths ----

// Arithmetic allowed for all arithmetic types EXCEPT plain char and bool.
// (signed char / unsigned char remain allowed; floats allowed; bool/char blocked)
template<typename X>
struct bml_is_math_arithmetic
    : std::bool_constant<
        std::is_arithmetic<X>::value &&
        !std::is_same<typename std::remove_cv<X>::type, char>::value &&
        !std::is_same<typename std::remove_cv<X>::type, bool>::value> {};

// Modulus allowed for integral types EXCEPT plain char and bool.
// (signed char / unsigned char remain allowed)
template<typename X>
struct bml_is_math_integral
    : std::bool_constant<
        std::is_integral<X>::value &&
        !std::is_same<typename std::remove_cv<X>::type, char>::value &&
        !std::is_same<typename std::remove_cv<X>::type, bool>::value> {};

template<typename T>
class Matrix
{
private:
    std::vector<std::vector<T>> data;
    unsigned int rows;
    unsigned int cols;

public:
    Matrix(unsigned int numRows, unsigned int numCols);

    Matrix(const Matrix<T>& oldMatrix);

    unsigned int numRows() const;

    unsigned int numCols() const;

    std::vector<T>& operator[](unsigned int row);

    const std::vector<T>& operator[](unsigned int row) const;

    void initFromByteStream(const char* byteStream, size_t byteSize);

    std::vector<char> toByteStream() const;

    Matrix copy(const unsigned int startRow = 0, const unsigned int startCol = 0, const int endRow = -1, const int endCol = -1) const;

    void paste(const Matrix& source, const unsigned int destRow = 0, const unsigned int destCol = 0);

    bool all(std::function<bool(T)> condition) const;

    Matrix<T> where(std::function<bool(T)> condition, T trueValue, T falseValue) const;

    Iterator<T> begin(TraversalType type = TraversalType::Row);
    Iterator<T> end(TraversalType type = TraversalType::Row);
    ConstIterator<T> begin(TraversalType type = TraversalType::Row) const;
    ConstIterator<T> end(TraversalType type = TraversalType::Row) const;

    std::string toString() const;

    std::vector<T> getRow(unsigned int row,  int startCol = 0,  int endCol = -1) const;

    std::vector<T> getColumn(unsigned int col, int startRow = 0, int endRow = -1) const;

    std::vector<T> getDiagonal( int start = 0,  int end = -1) const;

    std::vector<T> getAntiDiagonal(int start = 0, int end = -1) const;

    void fill(const T& value);

    // ---- Matrix-matrix arithmetic (char/bool excluded; signed/unsigned char allowed) ----
    template<typename U = T>
    typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
    operator+(const Matrix<T>& other) const;

    template<typename U = T>
    typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
    operator-(const Matrix<T>& other) const;

    template<typename U = T>
    typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
    operator*(const Matrix<T>& other) const;

    template<typename U = T>
    typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
    operator/(const Matrix<T>& other) const;

    // ---- Modulus only for "real" integrals (plain char/bool excluded) ----
    template<typename U = T>
    typename std::enable_if<bml_is_math_integral<U>::value, Matrix<T>>::type
    operator%(const Matrix<T>& other) const;

    // ---- Scalar arithmetic (char/bool excluded; signed/unsigned char allowed) ----
    template<typename U = T>
    typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
    operator+(const T& scalar) const;

    template<typename U = T>
    typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
    operator-(const T& scalar) const;

    template<typename U = T>
    typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
    operator*(const T& scalar) const;

    template<typename U = T>
    typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
    operator/(const T& scalar) const;

    // ---- Scalar modulus only for "real" integrals (plain char/bool excluded) ----
    template<typename U = T>
    typename std::enable_if<bml_is_math_integral<U>::value, Matrix<T>>::type
    operator%(const T& scalar) const;
};

// Equality operators (available for all T)
template<typename T>
bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs);

template<typename T>
bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs);

// Relational operators: DISABLED for pointer T (e.g., void*, const void*)
template<typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
bool operator<(const Matrix<T>& lhs, const Matrix<T>& rhs);

template<typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
bool operator>(const Matrix<T>& lhs, const Matrix<T>& rhs);

template<typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
bool operator<=(const Matrix<T>& lhs, const Matrix<T>& rhs);

template<typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
bool operator>=(const Matrix<T>& lhs, const Matrix<T>& rhs);

extern void testMatrix(void);

#endif // BML_HPP_INCLUDED
