#ifndef ITERATOR_HPP
#define ITERATOR_HPP
#include <tuple>
#include <cstdint>
template <typename T> class Matrix;
// Helper aliases: deduce element access types
template<typename T>
using element_ref_t =
    decltype(std::declval<Matrix<T>&>()[0][0]);            // e.g. T& or BoolRef

template<typename T>
using const_element_t =
    decltype(std::declval<const Matrix<T>&>()[0][0]);      // e.g. const T& or bool


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
    std::tuple<std::uint32_t, std::uint32_t, const_element_t<T>> operator*() const;
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
    Iterator(Matrix<T>& mat, std::int64_t r, std::int64_t c, TraversalType traversalType = TraversalType::Row);

    Iterator& operator++();
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;
    std::tuple<std::uint32_t, std::uint32_t, element_ref_t<T>> operator*() const;
};


#endif // ITERATOR_HPP
