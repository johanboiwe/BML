#ifndef ITERATOR_HPP
#define ITERATOR_HPP
#include <tuple>
template <typename T> class Matrix;
enum class TraversalType
{
    Row,          // Traverse row by row
    Column,       // Traverse column by column
    Diagonal,     // Traverse diagonally
    AntiDiagonal  // Traverse anti-diagonally
};
template<typename T>
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
template<typename T>
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


#endif // ITERATOR_HPP
