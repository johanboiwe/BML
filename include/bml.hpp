#ifndef BML_HPP_INCLUDED
#define BML_HPP_INCLUDED

#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <functional>
#include <type_traits>
#include "iterator.hpp"
#include "rowView.hpp"
#if defined(__SIZEOF_POINTER__)
#if __SIZEOF_POINTER__ != 8
#error "BML requires a 64-bit target (pointer size must be 8 bytes)."
#endif
#elif defined(_WIN64) || defined(__LP64__) || defined(_LP64)
// OK
#else
#error "BML requires a 64-bit target (could not verify pointer size)."
#endif


template<class U> struct storage_of
{
    using type = U;
};
template<>        struct storage_of<bool>
{
    using type = std::uint8_t;
};
template<class U> using storage_of_t = typename storage_of<U>::type;

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
template<typename X>
struct bml_is_bool : std::is_same<typename std::remove_cv<X>::type, bool> {};


template<typename T>
class Matrix
{
private:
// variables
    using store_t = storage_of_t<T>;
    std::vector<store_t> data;
    std::uint32_t rows;
    std::uint32_t cols;

    // private methods
    inline std::size_t toIdx(std::uint32_t r, std::uint32_t c) const noexcept;
    [[nodiscard]] std::pair<std::uint32_t, std::uint32_t> toCoords(std::size_t i) const;


public:
    Matrix(std::uint32_t numRows, std::uint32_t numCols);

    Matrix(const Matrix<T>&) = default;


    std::uint32_t numRows() const;

    std::uint32_t numCols() const;

    size_t size() const noexcept;
    bool empty() const noexcept;
    bool any_of(std::function<bool(T)> p) const;
    bool none_of(std::function<bool(T)> p) const;


    RowView<T> operator[](std::uint32_t row);
    RowView<const T> operator[](std::uint32_t row) const;

    void initFromByteStream(const uint8_t* byteStream, size_t byteSize);
    void initFromByteStream(const std::vector<uint8_t>& byteStream);

    std::vector<std::uint8_t> toByteStream() const;

    Matrix copy(const std::uint32_t startRow = 0, const std::uint32_t startCol = 0, const std::int32_t endRow = -1, const std::int32_t endCol = -1) const;

    void paste(const Matrix& source, const std::uint32_t destRow = 0, const std::uint32_t destCol = 0);

    bool all(std::function<bool(T)> condition) const;

    Matrix<T> where(std::function<bool(T)> condition, T trueValue, T falseValue) const;

    Iterator<T> begin(TraversalType type = TraversalType::Row);
    Iterator<T> end(TraversalType type = TraversalType::Row);
    ConstIterator<T> begin(TraversalType type = TraversalType::Row) const;
    ConstIterator<T> end(TraversalType type = TraversalType::Row) const;

    std::string toString() const;

    std::vector<T> getRow(std::uint32_t row,  std::int32_t startCol = 0,  std::int32_t endCol = -1) const;

    std::vector<T> getColumn(std::uint32_t col, std::int32_t startRow = 0, std::int32_t endRow = -1) const;

    std::vector<T> getDiagonal(std::int32_t start = 0,  std::int32_t end = -1) const;

    std::vector<T> getAntiDiagonal(std::int32_t start = 0, std::int32_t end = -1) const;

    void fill(const T& value);

    // --- Reductions (SFINAE-gated) ---
// sum() – floating point (Kahan)
    template<typename U = T>
    std::enable_if_t<std::is_floating_point<U>::value, T>
    sum() const;

// sum() – non-floating arithmetic (ints, signed/unsigned char)
    template<typename U = T>
    std::enable_if_t<bml_is_math_arithmetic<U>::value && !std::is_floating_point<U>::value, T>
    sum() const;                       // Kahan for floats

    template<typename U = T>
    typename std::enable_if<!std::is_pointer<U>::value, T>::type
    min() const;                        // throws on empty

    template<typename U = T>
    typename std::enable_if<!std::is_pointer<U>::value, T>::type
    max() const;

    // Indices of min/max element (row, col). Throws on empty.
    template<typename U = T>
    std::enable_if_t<!std::is_pointer<U>::value,
        std::pair<std::uint32_t, std::uint32_t>>
        argmin() const;

    template<typename U = T>
    std::enable_if_t<!std::is_pointer<U>::value,
        std::pair<std::uint32_t, std::uint32_t>>
        argmax() const;


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

    // ---------- (1) Compound assignment: Matrix ⊕= Matrix ----------
    template<typename U=T> std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
    operator+=(const Matrix& other);
    template<typename U=T> std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
    operator-=(const Matrix& other);
    template<typename U=T> std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
    operator*=(const Matrix& other);
    template<typename U=T> std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
    operator/=(const Matrix& other);
    template<typename U=T> std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator%=(const Matrix& other);

// ---------- (1) Compound assignment: Matrix ⊕= scalar ----------
    template<typename U=T> std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
    operator+=(const T& s);
    template<typename U=T> std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
    operator-=(const T& s);
    template<typename U=T> std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
    operator*=(const T& s);
    template<typename U=T> std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
    operator/=(const T& s);
    template<typename U=T> std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator%=(const T& s);


// ---------- Bitwise element-wise (Matrix ⊗ Matrix) ----------
    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator&(const Matrix& other) const;

    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator|(const Matrix& other) const;

    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator^(const Matrix& other) const;

// ---------- Bitwise element-wise (Matrix ⊗ scalar) ----------
    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator&(const T& s) const;

    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator|(const T& s) const;

    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator^(const T& s) const;

// ---------- Compound bitwise ----------
// ---------- Compound bitwise ----------
    template<typename U = T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator&=(const Matrix& other);

    template<typename U = T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator|=(const Matrix& other);

    template<typename U = T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator^=(const Matrix& other);

    template<typename U = T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator&=(const T& s);

    template<typename U = T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator|=(const T& s);

    template<typename U = T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator^=(const T& s);

// ---------- Unary bitwise NOT ----------
    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator~() const;

// ---------- Shifts (integrals only) ----------
    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator<<(int k) const;

    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
    operator>>(int k) const;

    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator<<=(int k);

    template<typename U=T>
    std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
    operator>>=(int k);

    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, Matrix>
    logical_and(const Matrix& other) const;

    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, Matrix>
    logical_or(const Matrix& other) const;

    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, Matrix>
    logical_xor(const Matrix& other) const;

    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, Matrix>
    logical_not() const;

// scalar versions (bool on the right)
    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, Matrix>
    logical_and(bool s) const;

    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, Matrix>
    logical_or(bool s) const;

    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, Matrix>
    logical_xor(bool s) const;

// reductions/convenience for masks
    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, std::size_t>
    count_true() const noexcept;

    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, bool>
    any() const noexcept;

    template<typename U = T>
    std::enable_if_t<bml_is_bool<U>::value, bool>
    none() const noexcept;
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


extern int testMatrix(void);

#endif // BML_HPP_INCLUDED
