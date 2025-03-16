#ifndef BML_HPP_INCLUDED
#define BML_HPP_INCLUDED

#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <functional>
#include <type_traits>

enum class TraversalType
{
    Row,          // Traverse row by row
    Column,       // Traverse column by column
    Diagonal,     // Traverse diagonally
    AntiDiagonal  // Traverse anti-diagonally
};

template<typename T>
class Matrix
{
private:
    std::vector<std::vector<T>> data;
    unsigned int rows;
    unsigned int cols;

public:
    class ConstIterator
    {
    private:
        // Store a const reference
        const Matrix<T>& matrix;
        long row;
        long col;
        TraversalType type;

    public:
        // Constructor (declaration only here)
        ConstIterator(const Matrix<T>& mat,
                      long r,
                      long c,
                      TraversalType traversalType = TraversalType::Row);

        // Pre-increment operator
        ConstIterator& operator++();

        // Comparison
        bool operator==(const ConstIterator& other) const;
        bool operator!=(const ConstIterator& other) const;

        // Dereference
        std::tuple<unsigned int, unsigned int, const T&> operator*() const;
    };
    class Iterator
    {
    private:
        Matrix<T>& matrix;
        long row;
        long col;
        TraversalType type;  // Add traversal type

    public:
        Iterator(Matrix<T>& mat, signed long r, signed long c, TraversalType traversalType = TraversalType::Row);

        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        std::tuple<unsigned int, unsigned int, T> operator*() const;
    };

    Matrix(unsigned int numRows, unsigned int numCols);

    Matrix(const Matrix<T>& oldMatrix);

    unsigned int numRows() const;

    unsigned int numCols() const;

    std::vector<T>& operator[](unsigned int row);

    const std::vector<T>& operator[](unsigned int row) const;

    void initFromByteStream(const char* byteStream, unsigned int byteSize);

    std::vector<char> toByteStream() const;

    Matrix copy(const unsigned int startRow = 0, const unsigned int startCol = 0, const int endRow = -1, const int endCol = -1) const;

    void paste(const Matrix& source, const unsigned int destRow = 0, const unsigned int destCol = 0);

    bool all(std::function<bool(T)> condition) const;

    Matrix<T> where(std::function<bool(T)> condition, T trueValue, T falseValue) const;

    Iterator begin(TraversalType type = TraversalType::Row);
    Iterator end(TraversalType type = TraversalType::Row);
    ConstIterator begin(TraversalType type = TraversalType::Row) const;
    ConstIterator end(TraversalType type = TraversalType::Row) const;

    std::string toString() const;

    std::vector<T> getRow(unsigned int row,  int startCol = 0,  int endCol = -1) const;

    std::vector<T> getColumn(unsigned int col, int startRow = 0, int endRow = -1) const;

    std::vector<T> getDiagonal( int start = 0,  int end = -1) const;

    std::vector<T> getAntiDiagonal(int start = 0, int end = -1) const;

    void fill(const T& value);

// Operator functions
    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
            operator+(const Matrix<T>& other) const;

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
            operator-(const Matrix<T>& other) const;

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
            operator*(const Matrix<T>& other) const;

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
            operator/(const Matrix<T>& other) const;

// Modulus operator only for integral types
    template<typename U = T>
    typename std::enable_if<std::is_integral<U>::value, Matrix<T>>::type
            operator%(const Matrix<T>& other) const;

// Scalar operations
    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
            operator+(const T& scalar) const;

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
            operator-(const T& scalar) const;

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
            operator*(const T& scalar) const;

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
            operator/(const T& scalar) const;

// Modulus operator only for integral types
    template<typename U = T>
    typename std::enable_if<std::is_integral<U>::value, Matrix<T>>::type
            operator%(const T& scalar) const;


};
// Equality operators
template<typename T>
bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs);

template<typename T>
bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs);

// Relational operators
template<typename T>
bool operator<(const Matrix<T>& lhs, const Matrix<T>& rhs);

template<typename T>
bool operator>(const Matrix<T>& lhs, const Matrix<T>& rhs);

template<typename T>
bool operator<=(const Matrix<T>& lhs, const Matrix<T>& rhs);

template<typename T>
bool operator>=(const Matrix<T>& lhs, const Matrix<T>& rhs);
extern void testMatrix(void);


#endif // BML_HPP_INCLUDED
