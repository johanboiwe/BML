#include "bml.hpp"
#include <limits>
#include <iostream>
#include <vector>
#include <sstream> // Required for stringstream
#include <string>
#include <type_traits>
void printByteVector(const std::vector<char>& byteVector)
{
    std::cout << "[DEBUG] byteVector contents: ";
    for (size_t i = 0; i < byteVector.size(); ++i)
    {
        // Cast the char to unsigned char and then to int to get the numeric value.
        std::cout << static_cast<int>(static_cast<unsigned char>(byteVector[i])) << " ";
    }
    std::cout << std::endl;
}


//=========================================================
// 1. Non-const Iterator Implementation
//    (Matches your snippet, plus an operator++ example)
//=========================================================
template<typename T>
Matrix<T>::Iterator::Iterator(Matrix<T>& mat, signed long r, signed long c, TraversalType traversalType)
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
bool Matrix<T>::Iterator::operator==(const Iterator& other) const
{
    return &matrix == &other.matrix && row == other.row && col == other.col;
}

template<typename T>
bool Matrix<T>::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

// Dereference
template<typename T>
std::tuple<unsigned int, unsigned int, T> Matrix<T>::Iterator::operator*() const
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
typename Matrix<T>::Iterator& Matrix<T>::Iterator::operator++()
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

// Non-const begin()
template<typename T>
typename Matrix<T>::Iterator Matrix<T>::begin(TraversalType t)
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
typename Matrix<T>::Iterator Matrix<T>::end(TraversalType t)
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

//=========================================================
// 2. ConstIterator Implementation
//    (Mirrors the non-const, but references const Matrix)
//=========================================================
template<typename T>
Matrix<T>::ConstIterator::ConstIterator(const Matrix<T>& mat,
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
bool Matrix<T>::ConstIterator::operator==(const ConstIterator& other) const
{
    return &matrix == &other.matrix && row == other.row && col == other.col;
}

template<typename T>
bool Matrix<T>::ConstIterator::operator!=(const ConstIterator& other) const
{
    return !(*this == other);
}

template<typename T>
std::tuple<unsigned int, unsigned int, const T&>
Matrix<T>::ConstIterator::operator*() const
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
typename Matrix<T>::ConstIterator& Matrix<T>::ConstIterator::operator++()
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

// Const begin()
template<typename T>
typename Matrix<T>::ConstIterator Matrix<T>::begin(TraversalType t) const
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
typename Matrix<T>::ConstIterator Matrix<T>::end(TraversalType t) const
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



template<typename T>
Matrix<T>::Matrix(unsigned int numRows, unsigned int numCols) : rows(numRows), cols(numCols)
{
    data.resize(numRows);  // Resize to create `numRows` empty rows

    for (unsigned int i = 0; i < numRows; ++i)
    {
        data[i].resize(numCols);  // Resize each row to have `numCols` columns
    }
}
template<typename T>
Matrix<T>::Matrix(const Matrix<T>& oldMatrix)
{
    rows = oldMatrix.numRows();
    cols = oldMatrix.numCols();

    // Debug print: matrix dimensions
    //std::cout << "[DEBUG] Copying Matrix: " << rows << " rows, " << cols << " cols" << std::endl;

    // Convert the old matrix to a byte stream
    std::vector<char> byteVector = oldMatrix.toByteStream();
    //printByteVector(byteVector);

    // Debug print: byte stream size
    // std::cout << "[DEBUG] Byte stream size: " << byteVector.size() << " bytes" <<"its data is" << std::endl;


    // Get a pointer to the underlying byte array
    char* byteStream = byteVector.data();



    // Initialise the matrix using the byte stream
    initFromByteStream(byteStream, byteVector.size());

}
template<typename T>
unsigned int Matrix<T>::numRows() const
{
    return rows;
}

template<typename T>
unsigned int Matrix<T>::numCols() const
{
    return cols;
}

template<typename T>
std::vector<T>& Matrix<T>::operator[](unsigned int row)
{
    return data[row];
}

template<typename T>
const std::vector<T>& Matrix<T>::operator[](unsigned int row) const
{
    return data[row];
}


template<typename T>
void Matrix<T>::initFromByteStream(const char* byteStream, unsigned int byteSize)
{
    if (byteSize != rows * cols * sizeof(T))
    {
        throw std::runtime_error("Invalid byte stream size");
    }

    // Resize the outer vector to have 'rows' elements.
    data.resize(rows);

    // For each row, ensure it has 'cols' elements and copy the corresponding data.
    for (unsigned int i = 0; i < rows; ++i)
    {
        // Resize the inner vector to 'cols'
        data[i].resize(cols);

        // Calculate the starting position for this row in the byte stream.
        const char* rowStart = byteStream + i * cols * sizeof(T);

        // Copy the row data into the inner vector.
        std::memcpy(data[i].data(), rowStart, cols * sizeof(T));
    }
}
template<>
void Matrix<bool>::initFromByteStream(const char* byteStream, unsigned int byteSize)
{
    if (byteSize != rows * cols * sizeof(bool))
    {
        throw std::runtime_error("Invalid byte stream size for Matrix<bool>");
    }

    // Resize the outer vector to have 'rows' elements.
    data.resize(rows);

    // For each row, resize the inner vector to 'cols'
    for (unsigned int i = 0; i < rows; ++i)
    {
        data[i].resize(cols);
        for (unsigned int j = 0; j < cols; ++j)
        {
            // Assume each bool is stored as a full byte in the byte stream.
            data[i][j] = (byteStream[i * cols + j] != 0);
        }
    }
}
template<>
std::vector<char> Matrix<bool>::toByteStream() const
{
    // Here, we assume we want to store one byte per boolean.
    std::vector<char> byteStream(rows * cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            // Convert the bool value to a char (1 for true, 0 for false)
            byteStream[i * cols + j] = data[i][j] ? 1 : 0;
        }
    }
    return byteStream;
}

template<typename T>
std::vector<char> Matrix<T>::toByteStream() const
{
    std::vector<char> byteStream(rows * cols * sizeof(T));
    // Iterate over each row and copy its data into the byte stream.
    for (unsigned int i = 0; i < rows; ++i)
    {
        std::memcpy(byteStream.data() + i * cols * sizeof(T),
                    data[i].data(),
                    cols * sizeof(T));
    }
    return byteStream;
}
template<typename T>
Matrix<T> Matrix<T>::copy(const unsigned int startRow, const unsigned int startCol, const int endRow, const int endCol) const
{
    //std::cout << "Copying matrix with parameters: startRow = " << startRow << ", startCol = " << startCol << ", endRow = " << endRow << ", endCol = " << endCol << std::endl;
    unsigned int copyEndRow = (endRow == -1) ? rows : static_cast<unsigned int>(endRow);

    unsigned int copyEndCol = (endCol == -1) ? cols : static_cast<unsigned int>(endCol);

    if (startRow < 0 || copyEndRow > rows)
    {
        throw std::out_of_range("Invalid copy indices: startRow or endRow is out of range. (startRow: " + std::to_string(startRow) + ", endRow: " + std::to_string(copyEndRow) + ", rows: " + std::to_string(rows) + ")");
    }
    if (startCol < 0 || copyEndCol > cols)
    {
        throw std::out_of_range("Invalid copy indices: startCol or endCol is out of range. (startCol: " + std::to_string(startCol) + ", endCol: " + std::to_string(copyEndCol) + ", cols: " + std::to_string(cols) + ")");
    }
    if (startRow > copyEndRow)
    {
        throw std::out_of_range("Invalid copy indices: startRow is greater than endRow. (startRow: " + std::to_string(startRow) + ", endRow: " + std::to_string(copyEndRow) + ")");
    }
    if (startCol >= copyEndCol)
    {
        throw std::out_of_range("Invalid copy indices: startCol is greater than or equal to endCol. (startCol: " + std::to_string(startCol) + ", endCol: " + std::to_string(copyEndCol) + ")");
    }

    Matrix result(copyEndRow - startRow, copyEndCol - startCol);
    for (unsigned int i = startRow, r = 0; i < copyEndRow; ++i, ++r)
    {
        for (unsigned int j = startCol, c = 0; j < copyEndCol; ++j, ++c)
        {
            result[r][c] = (*this)[i][j];
        }
    }
    return result;
}

template<typename T>
void Matrix<T>::paste(const Matrix& source, const unsigned int destRow, const unsigned int destCol)
{
    if (destRow < 0 || destRow + source.numRows() > rows || destCol < 0 || destCol + source.numCols() > cols)
    {
        throw std::out_of_range("Invalid paste indices");
    }

    for (unsigned int i = 0; i < source.numRows(); ++i)
    {
        for (unsigned int j = 0; j < source.numCols(); ++j)
        {
            (*this)[destRow + i][destCol + j] = source[i][j];
        }
    }
}

template<typename T>
bool Matrix<T>::all(std::function<bool(T)> condition) const
{
    for (const auto& row : data)
    {
        for (const auto& element : row)
        {
            if (!condition(element))
            {
                return false;
            }
        }
    }
    return true;
}

template<typename T>
Matrix<T> Matrix<T>::where(std::function<bool(T)> condition, T trueValue, T falseValue) const
{
    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = condition((*this)[i][j]) ? trueValue : falseValue;
        }
    }
    return result;
}





template<typename T>
std::string Matrix<T>::toString() const
{
    std::stringstream ss;
    for (const auto& row : data)
    {
        for (const auto& element : row)
        {
            ss << element << ' ';
        }
        ss << "\n";
    }
    return ss.str();
}


template<typename T>
std::vector<T> Matrix<T>::getRow(unsigned int row, int startCol, int endCol) const
{
    if (row < 0 || row >= rows)
    {
        throw std::out_of_range("Invalid row index");
    }

    if (endCol == -1)
    {
        endCol = static_cast<int>(cols);

    }

    if (startCol < 0 || endCol > static_cast<int>(cols) || startCol >= endCol)

    {
        throw std::out_of_range("Invalid column slice indices");
    }

    return std::vector<T>(data[row].begin() + startCol, data[row].begin() + endCol);
}

template<typename T>
std::vector<T> Matrix<T>::getColumn(unsigned int col,  int startRow,  int endRow) const
{
    if (col < 0 || col >= cols)
    {
        throw std::out_of_range("Invalid column index");
    }

    if (endRow == -1)
    {
        endRow = static_cast<int>(rows);
    }

    if (startRow < 0 || static_cast<unsigned int>(endRow) > rows || startRow >= endRow)
    {
        throw std::out_of_range("Invalid row slice indices");
    }

    std::vector<T> result;
    result.reserve(static_cast<size_t>(std::abs(endRow - startRow)));

    for (int i = startRow; i < endRow; ++i)
    {
        result.push_back(data[static_cast<size_t>(i)][static_cast<size_t>(col)]);

    }
    return result;
}

template<typename T>
std::vector<T> Matrix<T>::getDiagonal( int start,  int end) const
{
    if (start < 0)
        if (start < 0)
        {
            start = 0;
        }

    if (end == -1)
    {
        end = static_cast<int>(std::min(rows, cols));

    }

    if (start >= end || static_cast<unsigned int>(start) >= rows || static_cast<unsigned int>(start) >= cols)
    {
        throw std::out_of_range("Invalid diagonal indices");
    }

    std::vector<T> result;
    result.reserve(static_cast<size_t>(std::abs(end - start)));
    for (int i = start; i < end; ++i)
    {
        result.push_back(data[static_cast<size_t>(i)][static_cast<size_t>(i)]);

    }
    return result;
}

template<typename T>
std::vector<T> Matrix<T>::getAntiDiagonal(int start, int end) const
{
    if (start < 0)
    {
        start = 0;
    }

    if (end == -1)
    {
        end = static_cast<int>(std::min(rows, cols));

    }

    if (start >= end || static_cast<unsigned int>(start) >= rows || static_cast<unsigned int>(start) >= cols)
    {
        throw std::out_of_range("Invalid anti-diagonal indices");
    }

    std::vector<T> result;
    result.reserve(static_cast<size_t>(std::abs(end - start)));
    for (int i = start; i < end; ++i)
    {
        result.push_back(data[static_cast<size_t>(i)][static_cast<unsigned int>(cols) - static_cast<unsigned int>(i) - 1]);

    }
    return result;
}

template<typename T>
void Matrix<T>::fill(const T& value)
{
    for (auto& row : data)
    {
        std::fill(row.begin(), row.end(), value);
    }
}

// Operator+ with another matrix
template<typename T>
template<typename U>
typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator+(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
    {
        throw std::invalid_argument("Matrix dimensions must match for addition.");
    }

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = data[i][j] + other.data[i][j];
        }
    }
    return result;
}

// Operator- with another matrix
template<typename T>
template<typename U>
typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator-(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
    {
        throw std::invalid_argument("Matrix dimensions must match for subtraction.");
    }

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = data[i][j] - other.data[i][j];
        }
    }
    return result;
}

// Operator* with another matrix
template<typename T>
template<typename U>
typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator*(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
    {
        throw std::invalid_argument("Matrix dimensions must match for multiplication.");
    }

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = data[i][j] * other.data[i][j];
        }
    }
    return result;
}

// Operator/ with another matrix
template<typename T>
template<typename U>
typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator/(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
    {
        throw std::invalid_argument("Matrix dimensions must match for division.");
    }

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            if (other.data[i][j] == 0)
            {
                throw std::runtime_error("Division by zero encountered.");
            }
            result[i][j] = data[i][j] / other.data[i][j];
        }
    }
    return result;
}

// Operator% with another matrix
template<typename T>
template<typename U>
typename std::enable_if<std::is_integral<U>::value, Matrix<T>>::type
        Matrix<T>::operator%(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
    {
        throw std::invalid_argument("Matrix dimensions must match for modulus.");
    }

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            if (other.data[i][j] == 0)
            {
                throw std::runtime_error("Modulus by zero encountered.");
            }
            result[i][j] = data[i][j] % other.data[i][j];
        }
    }
    return result;
}

// Scalar operations

// Operator+ with a scalar
template<typename T>
template<typename U>
typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator+(const T& scalar) const
{
    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = data[i][j] + scalar;
        }
    }
    return result;
}

// Operator- with a scalar
template<typename T>
template<typename U>
typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator-(const T& scalar) const
{
    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = data[i][j] - scalar;
        }
    }
    return result;
}

// Operator* with a scalar
template<typename T>
template<typename U>
typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator*(const T& scalar) const
{
    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = data[i][j] * scalar;
        }
    }
    return result;
}

// Operator/ with a scalar
template<typename T>
template<typename U>
typename std::enable_if<std::is_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator/(const T& scalar) const
{
    if (scalar == 0)
    {
        throw std::runtime_error("Division by zero encountered.");
    }

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)    // Fix this loop
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = data[i][j] / scalar;
        }
    }
    return result;
}
// Definition of matrix-to-scalar modulus operator
template<typename T>
template<typename U>
typename std::enable_if<std::is_integral<U>::value, Matrix<T>>::type
        Matrix<T>::operator%(const T& scalar) const
{
    if (scalar == 0)
    {
        throw std::runtime_error("Modulus by zero encountered.");
    }

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < cols; ++j)
        {
            result[i][j] = data[i][j] % scalar;
        }
    }
    return result;
}
// Note: If you plan to use these operators with many types,
// you'll need to explicitly instantiate them for each type,
// or include this source file at the bottom of your header.

// Equality operator: two matrices are equal if they have the same dimensions
// and all corresponding elements are equal.
template<typename T>
bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.numRows() != rhs.numRows() || lhs.numCols() != rhs.numCols())
        return false;
    for (unsigned int i = 0; i < lhs.numRows(); ++i)
    {
        for (unsigned int j = 0; j < lhs.numCols(); ++j)
        {
            if (lhs[i][j] != rhs[i][j])
                return false;
        }
    }
    return true;
}

// Inequality operator
template<typename T>
bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return !(lhs == rhs);
}

// Less-than operator: define a lexicographical order
template<typename T>
bool operator<(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.numRows() != rhs.numRows())
        return lhs.numRows() < rhs.numRows();
    if (lhs.numCols() != rhs.numCols())
        return lhs.numCols() < rhs.numCols();
    for (unsigned int i = 0; i < lhs.numRows(); ++i)
    {
        for (unsigned int j = 0; j < lhs.numCols(); ++j)
        {
            if (lhs[i][j] != rhs[i][j])
                return lhs[i][j] < rhs[i][j];
        }
    }
    return false;  // They are equal.
}

// Greater-than operator
template<typename T>
bool operator>(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return rhs < lhs;
}

// Less-than-or-equal-to operator
template<typename T>
bool operator<=(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return !(rhs < lhs);
}

// Greater-than-or-equal-to operator
template<typename T>
bool operator>=(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return !(lhs < rhs);
}

#define INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(T)                          \
    template bool operator==<T>(const Matrix<T>&, const Matrix<T>&);        \
    template bool operator!=<T>(const Matrix<T>&, const Matrix<T>&);        \
    template bool operator< <T>(const Matrix<T>&, const Matrix<T>&);         \
    template bool operator> <T>(const Matrix<T>&, const Matrix<T>&);         \
    template bool operator<=<T>(const Matrix<T>&, const Matrix<T>&);         \
    template bool operator>=<T>(const Matrix<T>&, const Matrix<T>&);

// Macro to instantiate the Matrix class and all operators except modulus for a specific type (T)
#define INSTANTIATE_MATRIX_OPERATORS_NO_MODULUS_FOR_TYPE(T)             \
    template class Matrix<T>;                                           \
    template Matrix<T> Matrix<T>::operator+(const Matrix<T>&) const;    \
    template Matrix<T> Matrix<T>::operator-(const Matrix<T>&) const;    \
    template Matrix<T> Matrix<T>::operator*(const Matrix<T>&) const;    \
    template Matrix<T> Matrix<T>::operator/(const Matrix<T>&) const;    \
    template Matrix<T> Matrix<T>::operator+(const T&) const;            \
    template Matrix<T> Matrix<T>::operator-(const T&) const;            \
    template Matrix<T> Matrix<T>::operator*(const T&) const;            \
    template Matrix<T> Matrix<T>::operator/(const T&) const;

// Macro to instantiate the modulus operator for integral types (like int)
#define INSTANTIATE_MATRIX_MODULUS_FOR_TYPE(T)                          \
    template Matrix<T> Matrix<T>::operator%(const Matrix<T>&) const;    \
    template Matrix<T> Matrix<T>::operator%(const T&) const;

// Macro to instantiate the Matrix class and all operators for integral types (includes modulus)
#define INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(T)               \
    INSTANTIATE_MATRIX_MODULUS_FOR_TYPE(T)                              \
    INSTANTIATE_MATRIX_OPERATORS_NO_MODULUS_FOR_TYPE(T)

// Macro to instantiate the Matrix class and all operators for floating-point types (no modulus)
#define INSTANTIATE_MATRIX_OPERATORS_FOR_FLOAT_TYPE(T)                  \
    INSTANTIATE_MATRIX_OPERATORS_NO_MODULUS_FOR_TYPE(T)
// Integer types
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(int)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(short)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(long)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(long long)

// Unsigned integer types
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(unsigned int)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(unsigned short)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(unsigned long)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(unsigned long long)

// Floating-point types
INSTANTIATE_MATRIX_OPERATORS_FOR_FLOAT_TYPE(float)
INSTANTIATE_MATRIX_OPERATORS_FOR_FLOAT_TYPE(double)
INSTANTIATE_MATRIX_OPERATORS_FOR_FLOAT_TYPE(long double)

// Character types
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(char)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(unsigned char)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(signed char)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(wchar_t)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(char16_t)
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(char32_t)

// Boolean type
INSTANTIATE_MATRIX_OPERATORS_FOR_INTEGRAL_TYPE(bool)

// special inits that does not have math functions
template class Matrix<std::string>;
template class Matrix<void*>;

// Instantiate for integral types:
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(int)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(short)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(long)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(long long)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(unsigned int)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(unsigned short)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(unsigned long)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(unsigned long long)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(bool)

// Instantiate for floating-point types:
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(float)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(double)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(long double)

// Instantiate for character types:
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(char)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(unsigned char)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(signed char)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(wchar_t)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(char16_t)
INSTANTIATE_MATRIX_COMPARISON_FOR_TYPE(char32_t)

// Optionally, instantiate for std::string and void*
// (Note: Arithmetic operations are not supported for these types,
// but if you want to use comparison operators, you can instantiate them.)
template bool operator==<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
template bool operator!=<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
template bool operator< <std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
template bool operator> <std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
template bool operator<=<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);
template bool operator>=<std::string>(const Matrix<std::string>&, const Matrix<std::string>&);

template bool operator==<void*>(const Matrix<void*>&, const Matrix<void*>&);
template bool operator!=<void*>(const Matrix<void*>&, const Matrix<void*>&);
template bool operator< <void*>(const Matrix<void*>&, const Matrix<void*>&);
template bool operator> <void*>(const Matrix<void*>&, const Matrix<void*>&);
template bool operator<=<void*>(const Matrix<void*>&, const Matrix<void*>&);
template bool operator>=<void*>(const Matrix<void*>&, const Matrix<void*>&);



