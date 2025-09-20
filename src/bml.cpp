#include "bml.hpp"
#include <limits>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <type_traits>
#include <algorithm> // std::min, std::fill
#include <cstring>   // std::memcpy

template<typename T>
inline std::size_t Matrix<T>::toIdx(unsigned int r, unsigned int c) const noexcept
{
    return static_cast<std::size_t>(r) * cols + c;
}

template<typename T>
std::pair<unsigned int, unsigned int> Matrix<T>::toCoords(std::size_t i) const
{
    const std::size_t n = static_cast<std::size_t>(rows) * cols;
    if (cols == 0) throw std::out_of_range("Matrix::toCoords: zero columns");
    if (i >= n)   throw std::out_of_range("Matrix::toCoords: index out of range");
    return { static_cast<unsigned int>(i / cols),
             static_cast<unsigned int>(i % cols) };
}

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
    data.resize(numRows* numCols);
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
RowView<T> Matrix<T>::operator[](unsigned int row)
{
    return RowView<T>(data.data()+ toIdx(row, 0), cols);
}

template<typename T>
RowView<const T> Matrix<T>::operator[](unsigned int row) const
{
    return RowView<const T>(data.data()+ toIdx(row, 0), cols);
}

template<typename T>
void Matrix<T>::initFromByteStream(const char* byteStream, size_t byteSize)
{
    if (byteSize != rows * cols * sizeof(T))
        throw std::runtime_error("Invalid byte stream size");

    std::memcpy(data.data(), byteStream, byteSize);
}



template<typename T>
std::vector<char> Matrix<T>::toByteStream() const
{
    std::vector<char> byteStream(rows * cols * sizeof(T));

    std::memcpy(byteStream.data(), data.data(), data.size());

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
    for (const auto& element : data)
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
// TODO (johan#1#): work with this when loops are made clear

    std::stringstream ss;
    return ss.str();
}

template<typename T>
std::vector<T> Matrix<T>::getRow(unsigned int row, int startCol, int endCol) const
{
    if (row >= rows)
        throw std::out_of_range("Invalid row index");

    if (endCol == -1) endCol = static_cast<int>(cols);

    if (startCol < 0 || endCol < 0)
        throw std::out_of_range("Negative column index");

    const unsigned int s = static_cast<unsigned int>(startCol);
    const unsigned int e = static_cast<unsigned int>(endCol);

    if (s > e || e > static_cast<unsigned int>(cols))
        throw std::out_of_range("Invalid column slice indices");

    return std::vector<T>(
               data.data() + toIdx(row, s),
               data.data() + toIdx(row, e)
           );
}



template<typename T>
std::vector<T> Matrix<T>::getColumn(unsigned int col, int startRow, int endRow) const
{
    if (col >= cols)
        throw std::out_of_range("Invalid column index");

    if (endRow == -1) endRow = static_cast<int>(rows);

    if (startRow < 0 || endRow < 0)
        throw std::out_of_range("Negative row index");

    const unsigned int s = static_cast<unsigned int>(startRow);
    const unsigned int e = static_cast<unsigned int>(endRow);

    if (s > e || e > static_cast<unsigned int>(rows))
        throw std::out_of_range("Invalid row slice indices");

    std::vector<T> result;
    result.reserve(static_cast<size_t>(e - s));

    for (unsigned int i = s; i < e; ++i)
        result.push_back(data[toIdx(i, col)]);

    return result;
}


template<typename T>
std::vector<T> Matrix<T>::getDiagonal(int start, int end) const
{
    if (start < 0) start = 0;
    const unsigned int limit = std::min(rows, cols);
    if (end == -1) end = static_cast<int>(limit);
    if (end < 0) throw std::out_of_range("Negative end index");

    const unsigned int s = static_cast<unsigned int>(start);
    const unsigned int e = static_cast<unsigned int>(end);

    if (s > e || e > limit)
        throw std::out_of_range("Invalid diagonal indices");

    std::vector<T> result;
    result.reserve(static_cast<size_t>(e - s));

    for (unsigned int i = s; i < e; ++i)
        result.push_back(data[toIdx(i, i)]);

    return result;
}


template<typename T>
std::vector<T> Matrix<T>::getAntiDiagonal(int start, int end) const
{
    if (start < 0) start = 0;

    const unsigned int limit = std::min(rows, cols);
    if (end == -1) end = static_cast<int>(limit);
    if (end < 0) throw std::out_of_range("Negative end index");

    const unsigned int s = static_cast<unsigned int>(start);
    const unsigned int e = static_cast<unsigned int>(end);

    if (s > e || e > limit)
        throw std::out_of_range("Invalid anti-diagonal indices");

    // Empty matrix or zero-width => empty result
    if (cols == 0 || rows == 0 || s == e)
        return {};

    std::vector<T> result;
    result.reserve(static_cast<size_t>(e - s));

    for (unsigned int i = s; i < e; ++i)
    {
        const unsigned int j = static_cast<unsigned int>(cols) - 1u - i;
        result.push_back(data[toIdx(i, j)]);
    }

    return result;
}


template<typename T>
void Matrix<T>::fill(const T& value)
{

    for (T& cell : data)
        cell = value;
}
template<>
void Matrix<bool>::fill(const bool& value)
{

    for (uint8_t& cell : data)
        cell = value;
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
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data.data()[i] = data.data()[i] + other.data.data()[i];
    }
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
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data.data()[i] = data.data()[i] - other.data.data()[i];
    }
    return result;
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
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data.data()[i] = data.data()[i] * other.data.data()[i];
    }
    return result;
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
    for(size_t i = 0; i < data.size(); i++)
    {
        if (other.data.data()[i] == 0)
            throw std::runtime_error("Division by zero encountered.");
        result.data.data()[i] = data.data()[i] / other.data.data()[i];
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
    for(size_t i = 0; i < data.size(); i++)
    {
        if (other.data[i] == 0) throw std::runtime_error("Modulus by zero encountered.");
        result.data.data()[i] = data.data()[i] % other.data.data()[i];
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
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data.data()[i] = data.data()[i] + scalar;
    }
    return result;
}

// Scalar -
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator-(const T& scalar) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data.data()[i] = data.data()[i] - scalar;
    }
    return result;
}

// Scalar *
template<typename T>
template<typename U>
typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        Matrix<T>::operator*(const T& scalar) const
{

    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data.data()[i] = data.data()[i] * scalar;
    }
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
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data.data()[i] = data.data()[i] / scalar;
    }
    return result;
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
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data.data()[i] = data.data()[i] % scalar;
    }
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

// -------------------- Reductions --------------------

template<typename T>
template<typename U>
std::enable_if_t<std::is_floating_point<U>::value, T>
Matrix<T>::sum() const
{
    if (data.empty()) return T{0}; // policy: 0 for empty

    T s = T{0};
    T c = T{0};

    for (const store_t& cell : data)
    {
        T y = static_cast<T>(cell) - c;
        T t = s + y;
        c = (t - s) - y;
        s = t;
    }
    return s;
}

// ---------- sum() for non-floating arithmetic ----------
template<typename T>
template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value && !std::is_floating_point<U>::value, T>
Matrix<T>::sum() const
{
    if (data.empty()) return T{0}; // policy: 0 for empty


    T sumValue = T{0};
    for (const store_t& cell : data)
    {
        sumValue += static_cast<T>(cell);
    }
    return sumValue;

}

template<typename T>
template<typename U>
typename std::enable_if<!std::is_pointer<U>::value, T>::type
Matrix<T>::min() const
{
    T minimalValue = data[0];
    for(auto& cell: data)
    {
        if (cell < minimalValue) minimalValue = cell;
    }
    return minimalValue;
}

template<typename T>
template<typename U>
typename std::enable_if<!std::is_pointer<U>::value, T>::type
Matrix<T>::max() const
{
    T maxValue = data[0];
    for(auto& cell: data)
    {
        if (cell > maxValue) maxValue = cell;
    }
    return maxValue;
}

template<typename T>
bool Matrix<T>::any_of(std::function<bool(T)> p) const
{
    for (const store_t& cell : data)
    {
        if (p(static_cast<T>(cell))) return true;
    }
    return false;
}

template<typename T>
bool Matrix<T>::none_of(std::function<bool(T)> p) const
{
    for (const store_t& cell : data)
    {
        if (p(static_cast<T>(cell))) return false;
    }
    return true;
}
template<typename T>
size_t Matrix<T>::size() const noexcept
{
    // element count, not bytes
    return static_cast<size_t>(rows) * static_cast<size_t>(cols);
}

template<typename T>
bool Matrix<T>::empty() const noexcept
{
    // true for 0x0, Rx0, or 0xC
    return rows == 0u || cols == 0u;
}

// ---------- argmin / argmax ----------

template<typename T>
template<typename U>
std::enable_if_t<!std::is_pointer<U>::value,
    std::pair<unsigned int, unsigned int>>
Matrix<T>::argmin() const
{
    if (data.empty())
        throw std::runtime_error("Matrix::argmin() on empty matrix");

    T minimalValue = static_cast<T>(data[0]);
    std::size_t minimalIndex = 0;
    std::size_t enumeration  = 0;

    for (const store_t& cell : data)
    {
        const T value = static_cast<T>(cell);
        if (value < minimalValue)
        {
            minimalValue = value;
            minimalIndex = enumeration;
        }
        enumeration++;
    }
    return toCoords(minimalIndex);
}

template<typename T>
template<typename U>
std::enable_if_t<!std::is_pointer<U>::value,
    std::pair<unsigned int, unsigned int>>
Matrix<T>::argmax() const
{
    if (data.empty())
        throw std::runtime_error("Matrix::argmax() on empty matrix");

    T maxValue = static_cast<T>(data[0]);
    std::size_t maxIndex    = 0;
    std::size_t enumeration = 0;

    for (const store_t& cell : data)
    {
        const T value = static_cast<T>(cell);
        if (value > maxValue)
        {
            maxValue = value;
            maxIndex = enumeration;
        }
        enumeration++;
    }
    return toCoords(maxIndex);
}


