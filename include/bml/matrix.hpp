#ifndef BML_MATRIX_HPP
#define BML_MATRIX_HPP

#include "bml/export.hpp"
#include "bml/typeTraits.hpp"
#include "bml/rowView.hpp"
#include "bml/traversal.hpp"

#include <vector>
#include <functional>
#include <string>
#include <type_traits>   // ensure enable_if / is_* are available

namespace bml
{

    template<class T> class MatrixIterator;
    template<class T> class ConstMatrixIterator;

    template <typename T>
    class BML_API Matrix
    {
        // Fail fast for completely unsupported storage types.
        static_assert(
            bml_is_bool<T>::value || bml_is_math_arithmetic<T>::value
            || std::is_same_v<T, char> || std::is_same_v<T, std::string>,
            "bml::Matrix<T>: unsupported T (allowed: bool, {u,}int{8,16,32,64}_t, "
            "float/double/long double, char, std::string)"
        );

    private:
        using store_t = storage_of_t<T>;
        std::vector<store_t> data;
        std::uint32_t rows;
        std::uint32_t cols;

        [[nodiscard]] std::size_t toIdx(std::uint32_t r, std::uint32_t c) const noexcept;
        [[nodiscard]] std::pair<std::uint32_t, std::uint32_t> toCoords(std::size_t i) const;

    public:
        Matrix(std::uint32_t numRows, std::uint32_t numCols);

        Matrix(const Matrix&) = default;
        Matrix& operator=(const Matrix&) = default;

        Matrix(Matrix&& other) noexcept;             // move ctor
        Matrix& operator=(Matrix&& other) noexcept;  // move assign
        ~Matrix() = default;

        [[nodiscard]] std::uint32_t numRows() const;
        [[nodiscard]] std::uint32_t numCols() const;

        [[nodiscard]] size_t size() const noexcept;
        [[nodiscard]] bool empty() const noexcept;

        bool any_of(std::function<bool(T)> p) const;
        bool none_of(std::function<bool(T)> p) const;

        RowView<T> operator[](std::uint32_t row);
        RowView<const T> operator[](std::uint32_t row) const;

        void initFromByteStream(const uint8_t* byteStream, size_t byteSize);
        void initFromByteStream(const std::vector<uint8_t>& byteStream);

        [[nodiscard]] std::vector<std::uint8_t> toByteStream() const;

        Matrix copy(std::uint32_t startRow = 0, std::uint32_t startCol = 0,
                    std::int32_t endRow = -1, std::int32_t endCol = -1) const;

        void paste(const Matrix& source, std::uint32_t destRow = 0, std::uint32_t destCol = 0);

        bool all(std::function<bool(T)> condition) const;

        Matrix<T> where(std::function<bool(T)> condition, T trueValue, T falseValue) const;

        MatrixIterator<T> begin(TraversalType type = TraversalType::Row);
        MatrixIterator<T> end(TraversalType type = TraversalType::Row);
        ConstMatrixIterator<T> begin(TraversalType type = TraversalType::Row) const;
        ConstMatrixIterator<T> end(TraversalType type = TraversalType::Row) const;

        [[nodiscard]] std::string toString() const;

        std::vector<T> getRow(std::uint32_t row, std::int32_t startCol = 0, std::int32_t endCol = -1) const;
        std::vector<T> getColumn(std::uint32_t col, std::int32_t startRow = 0, std::int32_t endRow = -1) const;
        std::vector<T> getDiagonal(std::int32_t start = 0, std::int32_t end = -1) const;
        std::vector<T> getAntiDiagonal(std::int32_t start = 0, std::int32_t end = -1) const;

        void fill(const T& value);

        // --- Reductions (SFINAE-gated) ---
        // sum() – floating point (Kahan)
        template <typename U = T>
        std::enable_if_t<std::is_floating_point<U>::value, T>
        sum() const;

        // sum() – non-floating arithmetic (ints, signed/unsigned char)
        template <typename U = T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value && !std::is_floating_point<U>::value, T>
        sum() const;

        // everything else → compile-time error
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, T>
        sum() const = delete;

        // min/max/arg* — disabled for pointer T
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value, T>::type
        min() const; // throws on empty

        template <typename U = T>
        typename std::enable_if<std::is_pointer<U>::value, T>::type
        min() const = delete;

        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value, T>::type
        max() const;

        template <typename U = T>
        typename std::enable_if<std::is_pointer<U>::value, T>::type
        max() const = delete;

        template <typename U = T>
        std::enable_if_t<!std::is_pointer<U>::value,
                         std::pair<std::uint32_t, std::uint32_t>>
        argmin() const;

        // pointer T → compile-time error
        template <typename U = T>
        std::enable_if_t<std::is_pointer<U>::value,
                         std::pair<std::uint32_t, std::uint32_t>>
        argmin() const = delete;

        template <typename U = T>
        std::enable_if_t<!std::is_pointer<U>::value,
                         std::pair<std::uint32_t, std::uint32_t>>
        argmax() const;

        template <typename U = T>
        std::enable_if_t<std::is_pointer<U>::value,
                         std::pair<std::uint32_t, std::uint32_t>>
        argmax() const = delete;

        // ---- Matrix-matrix arithmetic (char/bool excluded; signed/unsigned char allowed) ----
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator+(const Matrix<T>& other) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator+(const Matrix<T>&) const = delete;

        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator-(const Matrix<T>& other) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator-(const Matrix<T>&) const = delete;

        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator*(const Matrix<T>& other) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator*(const Matrix<T>&) const = delete;

        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator/(const Matrix<T>& other) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator/(const Matrix<T>&) const = delete;

        // ---- Scalar arithmetic (char/bool excluded; signed/unsigned char allowed) ----
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator+(const T& scalar) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator+(const T&) const = delete;

        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator-(const T& scalar) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator-(const T&) const = delete;

        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator*(const T& scalar) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator*(const T&) const = delete;

        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator/(const T& scalar) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator/(const T&) const = delete;

        // ---- Modulus only for "real" integrals (plain char/bool excluded) ----
        template <typename U = T>
        typename std::enable_if<bml_is_math_integral<U>::value, Matrix<T>>::type
        operator%(const Matrix<T>& other) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix<T>>
        operator%(const Matrix<T>&) const = delete;

        template <typename U = T>
        typename std::enable_if<bml_is_math_integral<U>::value, Matrix<T>>::type
        operator%(const T& scalar) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix<T>>
        operator%(const T&) const = delete;

        // ---------- (1) Compound assignment: Matrix ⊕= Matrix ----------
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator+=(const Matrix& other);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator+=(const Matrix&) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator-=(const Matrix& other);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator-=(const Matrix&) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator*=(const Matrix& other);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator*=(const Matrix&) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator/=(const Matrix& other);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator/=(const Matrix&) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator%=(const Matrix& other);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator%=(const Matrix&) = delete;

        // ---------- (1) Compound assignment: Matrix ⊕= scalar ----------
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator+=(const T& s);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator+=(const T&) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator-=(const T& s);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator-=(const T&) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator*=(const T& s);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator*=(const T&) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator/=(const T& s);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator/=(const T&) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator%=(const T& s);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator%=(const T&) = delete;

        // ---------- Bitwise element-wise (Matrix ⊗ Matrix) ----------
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator&(const Matrix& other) const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator&(const Matrix&) const = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator|(const Matrix& other) const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator|(const Matrix&) const = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator^(const Matrix& other) const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator^(const Matrix&) const = delete;

        // ---------- Bitwise element-wise (Matrix ⊗ scalar) ----------
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator&(const T& s) const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator&(const T&) const = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator|(const T& s) const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator|(const T&) const = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator^(const T& s) const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator^(const T&) const = delete;

        // ---------- Compound bitwise ----------
        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator&=(const Matrix& other);
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator&=(const Matrix&) = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator|=(const Matrix& other);
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator|=(const Matrix&) = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator^=(const Matrix& other);
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator^=(const Matrix&) = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator&=(const T& s);
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator&=(const T&) = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator|=(const T& s);
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator|=(const T&) = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator^=(const T& s);
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator^=(const T&) = delete;

        // ---------- Unary bitwise NOT ----------
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator~() const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator~() const = delete;

        // ---------- Shifts (integrals only) ----------
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator<<(int k) const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator<<(int) const = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator>>(int k) const;
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator>>(int) const = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator<<=(int k);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator<<=(int) = delete;

        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator>>=(int k);
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator>>=(int) = delete;

        // ---------- Bool logical (only for T=bool) ----------
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_and(const Matrix& other) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_and(const Matrix&) const = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_or(const Matrix& other) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_or(const Matrix&) const = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_xor(const Matrix& other) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_xor(const Matrix&) const = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_not() const;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_not() const = delete;

        // scalar versions (bool on the right)
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_and(bool s) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_and(bool) const = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_or(bool s) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_or(bool) const = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_xor(bool s) const;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_xor(bool) const = delete;

        // reductions/convenience for masks
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, std::size_t>
        count_true() const noexcept;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, std::size_t>
        count_true() const noexcept = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, bool>
        any() const noexcept;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, bool>
        any() const noexcept = delete;

        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, bool>
        none() const noexcept;
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, bool>
        none() const noexcept = delete;
    };

    // Equality operators (available for all T)
    template <typename T>
    bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs);

    template <typename T>
    bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs);

    // Relational operators: DISABLED for pointer T (e.g., void*, const void*)
    template <typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
    bool operator<(const Matrix<T>& lhs, const Matrix<T>& rhs);

    template <typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
    bool operator>(const Matrix<T>& lhs, const Matrix<T>& rhs);

    template <typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
    bool operator<=(const Matrix<T>& lhs, const Matrix<T>& rhs);

    template <typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
    bool operator>=(const Matrix<T>& lhs, const Matrix<T>& rhs);

} // namespace bml

#endif // BML_MATRIX_HPP
