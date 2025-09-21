#include "iterator.hpp"
#include "bml.hpp"
#include <limits>


template<typename T>
Iterator<T>::Iterator(Matrix<T>& mat, std::int64_t r, std::int64_t c, TraversalType traversalType)
    : matrix(mat), row(r), col(c), type(traversalType)
{
    if (row > static_cast<long>(std::numeric_limits<int>::max()))
    {
        throw std::runtime_error(
            "Dimensions can be max " + std::to_string(std::numeric_limits<int>::max())
        );
    }
}

// Equality
template<typename T>
bool Iterator<T>::operator==(const Iterator& other) const
{
    return &matrix == &other.matrix
        && type == other.type
        && row == other.row
        && col == other.col;
}

template<typename T>
bool Iterator<T>::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

// Dereference
template<typename T>
std::tuple<std::uint32_t, std::uint32_t, T> Iterator<T>::operator*() const
{
    if (row >= static_cast<long>(matrix.numRows()) ||
            col >= static_cast<long>(matrix.numCols()) ||
            row < 0 || col < 0)
    {
        throw std::out_of_range("Iterator out of bounds");
    }
    return std::make_tuple(static_cast<std::uint32_t>(row),
                           static_cast<std::uint32_t>(col),
                           matrix[static_cast<std::uint32_t>(row)][static_cast<std::uint32_t>(col)]);
}

// Example operator++ (if you want row-wise, column-wise, diagonal, etc.)
template<typename T>
Iterator<T>& Iterator<T>::operator++()
{
    switch (type)
    {
    case TraversalType::Row:
    {
        ++col;
        if (col >= static_cast<long>(matrix.numCols()))
        {
            col = 0;
            ++row;
        }
        break;
    }
    case TraversalType::Column:
    {
        ++row;
        if (row >= static_cast<long>(matrix.numRows()))
        {
            row = 0;
            ++col;
        }
        break;
    }
    case TraversalType::Diagonal:
    {
        ++row;
        ++col;
        break;
    }
    case TraversalType::AntiDiagonal:
    {
        ++row;
        --col;
        break;
    }
    }
    return *this;
}

//=========================================================
// 2. ConstIterator Implementation
//    (Mirrors the non-const, but references const Matrix)
//=========================================================
template<typename T>
ConstIterator<T>::ConstIterator(const Matrix<T>& mat,
                                        std::int64_t r,
                                        std::int64_t c,
                                        TraversalType traversalType)
    : matrix(mat), row(r), col(c), type(traversalType)
{
    if (row > static_cast<long>(std::numeric_limits<int>::max()))
    {
        throw std::runtime_error(
            "Dimensions can be max " + std::to_string(std::numeric_limits<int>::max())
        );
    }
}

template<typename T>
bool ConstIterator<T>::operator==(const ConstIterator& other) const
{
    return &matrix == &other.matrix
        && type == other.type
        && row == other.row
        && col == other.col;
}

template<typename T>
bool ConstIterator<T>::operator!=(const ConstIterator& other) const
{
    return !(*this == other);
}

template<typename T>
std::tuple<std::uint32_t, std::uint32_t, const T&>
ConstIterator<T>::operator*() const
{
    // Optional debug prints
    //  std::cerr << "DEBUG ConstIterator::operator*()"
    //          << " row=" << row
    //        << " col=" << col;

    // Check if indices are in range
    if (row >= static_cast<long>(matrix.numRows()) ||
            col >= static_cast<long>(matrix.numCols()) ||
            row < 0 || col < 0)
    {
        std::cerr << " -- OUT OF BOUNDS!\n";
        throw std::out_of_range("ConstIterator out of bounds");
    }

    // Print the address of the element in the underlying vector
    const T& ref = matrix[static_cast<std::uint32_t>(row)][static_cast<std::uint32_t>(col)];
//   std::cerr << " value=" << ref
    //           << " &value=" << static_cast<const void*>(&ref)
    //         << std::endl;

    // Return the tuple with the reference
    return
    {
        static_cast<std::uint32_t>(row),
        static_cast<std::uint32_t>(col),
        ref
    };

}

// Same traversal logic as the non-const operator++
// (increment row/col according to type)

