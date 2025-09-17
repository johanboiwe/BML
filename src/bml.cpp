#include "bml.hpp"
#include <limits>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <type_traits>
#include <algorithm> // std::min, std::fill
#include <cstring>   // std::memcpy

// ======================= Iterators =======================

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

template<typename T>
Iterator<T> Matrix<T>::end(TraversalType t)
{
    switch (t)
    {
    case TraversalType::Row:
        return Iterator(*this, static_cast<long>(numRows()), 0, TraversalType::Row);
    case TraversalType::Column:
        return Iterator(*this, 0, static_cast<long>(numCols()), TraversalType::Column);
    case TraversalType::Diagonal:
        return Iterator(*this,
                        static_cast<long>(std::min(numRows(), numCols())),
                        static_cast<long>(std::min(numRows(), numCols())),
                        TraversalType::Diagonal);
    case TraversalType::AntiDiagonal:
        return Iterator(*this,
                        static_cast<long>(std::min(numRows(), numCols())),
                        -1L,
                        TraversalType::AntiDiagonal);
    default:
        return Iterator(*this, static_cast<long>(numRows()), 0, TraversalType::Row);
    }
}

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

template<typename T>
ConstIterator<T> Matrix<T>::end(TraversalType t) const
{
    switch (t)
    {
    case TraversalType::Row:
        return ConstIterator(*this, static_cast<long>(numRows()), 0, TraversalType::Row);
    case TraversalType::Column:
        return ConstIterator(*this, 0, static_cast<long>(numCols()), TraversalType::Column);
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
        return ConstIterator(*this, static_cast<long>(numRows()), 0, TraversalType::Row);
    }
}

// ======================= Core =======================

template<typename T>
Matrix<T>::Matrix(unsigned int numRows, unsigned int numCols)
    : rows(numRows), cols(numCols)
{
    data.resize(numRows);
    for (unsigned int i = 0; i < numRows; ++i)
        data[i].resize(numCols);
}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& oldMatrix)
{
    rows = oldMatrix.numRows();
    cols = oldMatrix.numCols();
    std::vector<char> byteVector = oldMatrix.toByteStream();
    char* byteStream = byteVector.data();
    initFromByteStream(byteStream, static_cast<unsigned int>(byteVector.size()));
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
void Matrix<T>::initFromByteStream(const char* byteStream, size_t byteSize)
{
    if (byteSize != rows * cols * sizeof(T))
        throw std::runtime_error("Invalid byte stream size");

    data.resize(rows);
    for (unsigned int i = 0; i < rows; ++i)
    {
        data[i].resize(cols);
        const char* rowStart = byteStream + i * cols * sizeof(T);
        std::memcpy(data[i].data(), rowStart, cols * sizeof(T));
    }
}

// Matrix<bool> specialisation for byte stream I/O
template<>
void Matrix<bool>::initFromByteStream(const char* byteStream, size_t byteSize)
{
    if (byteSize != rows * cols * sizeof(bool))
        throw std::runtime_error("Invalid byte stream size for Matrix<bool>");

    data.resize(rows);
    for (unsigned int i = 0; i < rows; ++i)
    {
        data[i].resize(cols);
        for (unsigned int j = 0; j < cols; ++j)
            data[i][j] = (byteStream[i * cols + j] != 0);
    }
}

template<>
std::vector<char> Matrix<bool>::toByteStream() const
{
    std::vector<char> byteStream(rows * cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            byteStream[i * cols + j] = data[i][j] ? 1 : 0;
    return byteStream;
}

template<typename T>
std::vector<char> Matrix<T>::toByteStream() const
{
    std::vector<char> byteStream(rows * cols * sizeof(T));
    for (unsigned int i = 0; i < rows; ++i)
    {
        std::memcpy(byteStream.data() + i * cols * sizeof(T),
                    data[i].data(),
                    cols * sizeof(T));
    }
    return byteStream;
}

template<>
void Matrix<std::string>::initFromByteStream(const char* byteStream, size_t byteSize)
{
    //finding the stat locations in the stream
    std::vector<size_t> startLocations;
    startLocations.push_back(0);
    for (size_t i = 0; i < byteSize; ++i)
    {
        if(byteStream[i] == 0) startLocations.push_back(i+1);
    }
    //check if the amount of strings is right
    if((startLocations.size()-1) != rows * cols)throw std::runtime_error("Invalid byte stream size for Matrix<std::string>");
    if(startLocations[startLocations.size()-1]-0 != byteSize)throw std::runtime_error("Invalid byte stream format for Matrix<std::string>, does not end with a null");

    // setting the matrixstrings
    size_t couter = 0;
    for (const auto& [row, col, _ ] : *this)
    {
        char* currentCStr = new char [startLocations[couter+1] - startLocations[couter]];
        std::memcpy(currentCStr, byteStream + startLocations[couter], startLocations[couter+1] - startLocations[couter]);
        (*this)[row][col] = std::string(currentCStr);
        delete[] currentCStr;
        couter++;
    }

}

template<>
std::vector<char> Matrix<std::string>::toByteStream() const
{
    std::vector<char> result;

    // (Optional) pre-reserve if you want fewer reallocations:
    // size_t total = 0;
    // for (const auto& [r, c, cell] : *this) total += cell.size() + 1;
    // result.reserve(total);

    for (const auto& [_, __, cell] : *this)
    {
        // append the string bytes
        result.insert(result.end(), cell.begin(), cell.end());
        // append NUL terminator so the parser can find the end
        result.push_back('\0');
    }

    return result;
}
template<typename T>
Matrix<T> Matrix<T>::copy(const unsigned int startRow,
                          const unsigned int startCol,
                          const int endRow,
                          const int endCol) const
{
    unsigned int copyEndRow = (endRow == -1) ? rows : static_cast<unsigned int>(endRow);
    unsigned int copyEndCol = (endCol == -1) ? cols : static_cast<unsigned int>(endCol);

    if (startRow < 0 || copyEndRow > rows)
        throw std::out_of_range("Invalid copy indices: row range");
    if (startCol < 0 || copyEndCol > cols)
        throw std::out_of_range("Invalid copy indices: col range");
    if (startRow > copyEndRow)
        throw std::out_of_range("Invalid copy indices: startRow > endRow");
    if (startCol >= copyEndCol)
        throw std::out_of_range("Invalid copy indices: startCol >= endCol");

    Matrix result(copyEndRow - startRow, copyEndCol - startCol);
    for (unsigned int i = startRow, r = 0; i < copyEndRow; ++i, ++r)
        for (unsigned int j = startCol, c = 0; j < copyEndCol; ++j, ++c)
            result[r][c] = (*this)[i][j];

    return result;
}

template<typename T>
void Matrix<T>::paste(const Matrix& source, const unsigned int destRow, const unsigned int destCol)
{
    if (destRow < 0 || destRow + source.numRows() > rows ||
            destCol < 0 || destCol + source.numCols() > cols)
        throw std::out_of_range("Invalid paste indices");

    for (unsigned int i = 0; i < source.numRows(); ++i)
        for (unsigned int j = 0; j < source.numCols(); ++j)
            (*this)[destRow + i][destCol + j] = source[i][j];
}

template<typename T>
bool Matrix<T>::all(std::function<bool(T)> condition) const
{
    for (const auto& row : data)
        for (const auto& element : row)
            if (!condition(element)) return false;
    return true;
}

template<typename T>
Matrix<T> Matrix<T>::where(std::function<bool(T)> condition, T trueValue, T falseValue) const
{
    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = condition((*this)[i][j]) ? trueValue : falseValue;
    return result;
}

template<typename T>
std::string Matrix<T>::toString() const
{
    std::stringstream ss;
    for (const auto& row : data)
    {
        for (const auto& element : row)
            ss << element << ' ';
        ss << "\n";
    }
    return ss.str();
}

template<typename T>
std::vector<T> Matrix<T>::getRow(unsigned int row, int startCol, int endCol) const
{
    if (row < 0 || row >= rows)
        throw std::out_of_range("Invalid row index");

    if (endCol == -1) endCol = static_cast<int>(cols);

    if (startCol < 0 || endCol > static_cast<int>(cols) || startCol >= endCol)
        throw std::out_of_range("Invalid column slice indices");

    return std::vector<T>(data[row].begin() + startCol, data[row].begin() + endCol);
}

template<typename T>
std::vector<T> Matrix<T>::getColumn(unsigned int col, int startRow, int endRow) const
{
    if (col < 0 || col >= cols)
        throw std::out_of_range("Invalid column index");

    if (endRow == -1) endRow = static_cast<int>(rows);

    if (startRow < 0 || static_cast<unsigned int>(endRow) > rows || startRow >= endRow)
        throw std::out_of_range("Invalid row slice indices");

    std::vector<T> result;
    result.reserve(static_cast<size_t>(std::abs(endRow - startRow)));

    for (int i = startRow; i < endRow; ++i)
        result.push_back(data[static_cast<size_t>(i)][static_cast<size_t>(col)]);

    return result;
}

template<typename T>
std::vector<T> Matrix<T>::getDiagonal(int start, int end) const
{
    if (start < 0) start = 0;
    if (end == -1) end = static_cast<int>(std::min(rows, cols));

    if (start >= end || static_cast<unsigned int>(start) >= rows || static_cast<unsigned int>(start) >= cols)
        throw std::out_of_range("Invalid diagonal indices");

    std::vector<T> result;
    result.reserve(static_cast<size_t>(std::abs(end - start)));
    for (int i = start; i < end; ++i)
        result.push_back(data[static_cast<size_t>(i)][static_cast<size_t>(i)]);
    return result;
}

template<typename T>
std::vector<T> Matrix<T>::getAntiDiagonal(int start, int end) const
{
    if (start < 0) start = 0;
    if (end == -1) end = static_cast<int>(std::min(rows, cols));

    if (start >= end || static_cast<unsigned int>(start) >= rows || static_cast<unsigned int>(start) >= cols)
        throw std::out_of_range("Invalid anti-diagonal indices");

    std::vector<T> result;
    result.reserve(static_cast<size_t>(std::abs(end - start)));
    for (int i = start; i < end; ++i)
        result.push_back(data[static_cast<size_t>(i)][static_cast<unsigned int>(cols) - static_cast<unsigned int>(i) - 1]);
    return result;
}

template<typename T>
void Matrix<T>::fill(const T& value)
{
    for (auto& row : data)
        std::fill(row.begin(), row.end(), value);
}

// ======================= Arithmetic (with refined SFINAE) =======================

// Matrix + Matrix
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator+(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match for addition.");

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = data[i][j] + other.data[i][j];
    return result;
}

// Matrix - Matrix
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator-(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match for subtraction.");

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = data[i][j] - other.data[i][j];
    return result;
}

// Matrix * Matrix
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator*(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match for multiplication.");

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = data[i][j] * other.data[i][j];
    return result;
}

// Matrix / Matrix
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator/(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match for division.");

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
        {
            if (other.data[i][j] == 0)
                throw std::runtime_error("Division by zero encountered.");
            result[i][j] = data[i][j] / other.data[i][j];
        }
    return result;
}

// Matrix % Matrix (integral, no plain char/bool)
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_integral<U>::value, Matrix<T>>::type
        Matrix<T>::operator%(const Matrix<T>& other) const
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match for modulus.");

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
        {
            if (other.data[i][j] == 0)
                throw std::runtime_error("Modulus by zero encountered.");
            result[i][j] = data[i][j] % other.data[i][j];
        }
    return result;
}

// Scalar +
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator+(const T& scalar) const
{
    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = data[i][j] + scalar;
    return result;
}

// Scalar -
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator-(const T& scalar) const
{
    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = data[i][j] - scalar;
    return result;
}

// Scalar *
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator*(const T& scalar) const
{
    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = data[i][j] * scalar;
    return result;
}

// Scalar /
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator/(const T& scalar) const
{
    if (scalar == 0)
        throw std::runtime_error("Division by zero encountered.");

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = data[i][j] / scalar;
    return result;
}

// Scalar % (integral, no plain char/bool)
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_integral<U>::value, Matrix<T>>::type
        Matrix<T>::operator%(const T& scalar) const
{
    if (scalar == 0)
        throw std::runtime_error("Modulus by zero encountered.");

    Matrix<T> result(rows, cols);
    for (unsigned int i = 0; i < rows; ++i)
        for (unsigned int j = 0; j < cols; ++j)
            result[i][j] = data[i][j] % scalar;
    return result;
}

// ======================= Comparisons =======================

// Equality
template<typename T>
bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.numRows() != rhs.numRows() || lhs.numCols() != rhs.numCols())
        return false;
    for (unsigned int i = 0; i < lhs.numRows(); ++i)
        for (unsigned int j = 0; j < lhs.numCols(); ++j)
            if (lhs[i][j] != rhs[i][j]) return false;
    return true;
}

// Inequality
template<typename T>
bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return !(lhs == rhs);
}

// Ordering: definition MUST NOT repeat the default template arg present in the header.
template<typename T, typename Enable>
bool operator<(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.numRows() != rhs.numRows())
        return lhs.numRows() < rhs.numRows();
    if (lhs.numCols() != rhs.numCols())
        return lhs.numCols() < rhs.numCols();
    for (unsigned int i = 0; i < lhs.numRows(); ++i)
        for (unsigned int j = 0; j < lhs.numCols(); ++j)
            if (lhs[i][j] != rhs[i][j])
                return lhs[i][j] < rhs[i][j];
    return false; // equal
}

template<typename T, typename Enable>
bool operator>(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return rhs < lhs;
}

template<typename T, typename Enable>
bool operator<=(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return !(rhs < lhs);
}

template<typename T, typename Enable>
bool operator>=(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return !(lhs < rhs);
}



