#include "iterator.hpp"
#include "bml.hpp"
#include <limits>


template<typename T>
Iterator<T>::Iterator(Matrix<T>& mat, signed long r, signed long c, TraversalType traversalType)
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
    return &matrix == &other.matrix && row == other.row && col == other.col;
}

template<typename T>
bool Iterator<T>::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

// Dereference
template<typename T>
std::tuple<unsigned int, unsigned int, T> Iterator<T>::operator*() const
{
    if (row >= static_cast<long>(matrix.numRows()) ||
            col >= static_cast<long>(matrix.numCols()) ||
            row < 0 || col < 0)
    {
        throw std::out_of_range("Iterator out of bounds");
    }
    return std::make_tuple(static_cast<unsigned int>(row),
                           static_cast<unsigned int>(col),
                           matrix[static_cast<unsigned int>(row)][static_cast<unsigned int>(col)]);
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
/*
// Non-const begin()
template<typename T>
Iterator<T> Matrix<T>::begin(TraversalType t)
{
    switch (t)
    {
    case TraversalType::Row:
        return Iterator(*this, 0, 0, TraversalType::Row);
    case TraversalType::Column:
        return Iterator(*this, 0, 0, TraversalType::Column);
    case TraversalType::Diagonal:
        return Iterator(*this, 0, 0, TraversalType::Diagonal);
    case TraversalType::AntiDiagonal:
        return Iterator(*this, 0, static_cast<long>(numCols()) - 1, TraversalType::AntiDiagonal);
    default:
        return Iterator(*this, 0, 0, TraversalType::Row);
    }
}

// Non-const end()
template<typename T>
Iterator<T> Matrix<T>::end(TraversalType t)
{
    switch (t)
    {
    case TraversalType::Row:
        // "One past" the last element in row-major
        return Iterator(*this, static_cast<long>(numRows()), 0, TraversalType::Row);
    case TraversalType::Column:
        // "One past" in column traversal
        return Iterator(*this, 0, static_cast<long>(numCols()), TraversalType::Column);
    case TraversalType::Diagonal:
        // "One past" in diagonal (min(rows, cols), min(rows, cols))
        return Iterator(*this,
                        static_cast<long>(std::min(numRows(), numCols())),
                        static_cast<long>(std::min(numRows(), numCols())),
                        TraversalType::Diagonal);
    case TraversalType::AntiDiagonal:
        // "One past" anti-diagonal
        return Iterator(*this,
                        static_cast<long>(std::min(numRows(), numCols())),
                        -1L,
                        TraversalType::AntiDiagonal);
    default:
        return Iterator(*this, static_cast<long>(numRows()), 0, TraversalType::Row);
    }
}
*/
//=========================================================
// 2. ConstIterator Implementation
//    (Mirrors the non-const, but references const Matrix)
//=========================================================
template<typename T>
ConstIterator<T>::ConstIterator(const Matrix<T>& mat,
                                        signed long r,
                                        signed long c,
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
    return &matrix == &other.matrix && row == other.row && col == other.col;
}

template<typename T>
bool ConstIterator<T>::operator!=(const ConstIterator& other) const
{
    return !(*this == other);
}

template<typename T>
std::tuple<unsigned int, unsigned int, const T&>
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
    const T& ref = matrix[static_cast<unsigned int>(row)][static_cast<unsigned int>(col)];
//   std::cerr << " value=" << ref
    //           << " &value=" << static_cast<const void*>(&ref)
    //         << std::endl;

    // Return the tuple with the reference
    return
    {
        static_cast<unsigned int>(row),
        static_cast<unsigned int>(col),
        ref
    };

}

// Same traversal logic as the non-const operator++
// (increment row/col according to type)
template<typename T>
ConstIterator<T>& ConstIterator<T>::operator++()
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
    //std::cout<< "this is: "<< this <<std::endl;
    return *this;
}
/*
// Const begin()
template<typename T>
ConstIterator<T> Matrix<T>::begin(TraversalType t) const
{
    switch (t)
    {
    case TraversalType::Row:
        return ConstIterator(*this, 0, 0, TraversalType::Row);
    case TraversalType::Column:
        return ConstIterator(*this, 0, 0, TraversalType::Column);
    case TraversalType::Diagonal:
        return ConstIterator(*this, 0, 0, TraversalType::Diagonal);
    case TraversalType::AntiDiagonal:
        return ConstIterator(*this, 0, static_cast<long>(numCols()) - 1, TraversalType::AntiDiagonal);
    default:
        return ConstIterator(*this, 0, 0, TraversalType::Row);
    }
}

// Const end()
template<typename T>
ConstIterator<T> Matrix<T>::end(TraversalType t) const
{
    switch (t)
    {
    case TraversalType::Row:
        return ConstIterator(*this,
                             static_cast<long>(numRows()),
                             0,
                             TraversalType::Row);
    case TraversalType::Column:
        return ConstIterator(*this,
                             0,
                             static_cast<long>(numCols()),
                             TraversalType::Column);
    case TraversalType::Diagonal:
        return ConstIterator(*this,
                             static_cast<long>(std::min(numRows(), numCols())),
                             static_cast<long>(std::min(numRows(), numCols())),
                             TraversalType::Diagonal);
    case TraversalType::AntiDiagonal:
        return ConstIterator(*this,
                             static_cast<long>(std::min(numRows(), numCols())),
                             -1L,
                             TraversalType::AntiDiagonal);
    default:
        return ConstIterator(*this,
                             static_cast<long>(numRows()),
                             0,
                             TraversalType::Row);
    }
}
*/
