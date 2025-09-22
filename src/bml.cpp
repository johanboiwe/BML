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
inline std::size_t Matrix<T>::toIdx(std::uint32_t r, std::uint32_t c) const noexcept
{
    return static_cast<std::size_t>(r) * cols + c;
}

template<typename T>
std::pair<std::uint32_t, std::uint32_t> Matrix<T>::toCoords(std::size_t i) const
{

    return { static_cast<std::uint32_t>(i / cols),
             static_cast<std::uint32_t>(i % cols) };
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

template<typename T>
Iterator<T> Matrix<T>::end(TraversalType t)
{
    const long R   = static_cast<long>(numRows());
    const long C   = static_cast<long>(numCols());
    const long len = std::min(R, C);

    switch (t)
    {
    case TraversalType::Row:
        // after (R-1, C-1): ++ -> (R, 0)
        return Iterator(*this, R, 0, TraversalType::Row);

    case TraversalType::Column:
        // after (R-1, C-1): ++ -> (0, C)
        return Iterator(*this, 0, C, TraversalType::Column);

    case TraversalType::Diagonal:
        // after (len-1, len-1): ++ -> (len, len)
        return Iterator(*this, len, len, TraversalType::Diagonal);

    case TraversalType::AntiDiagonal:
    {
        // start is typically (0, C-1)
        // last valid is (len-1, C-len)
        // post-increment end is (len, C-len-1)
        const long endRow = len;
        const long endCol = C - len - 1;   // NOTE: can be -1 only when C == len (i.e., C <= R)
        return Iterator(*this, endRow, endCol, TraversalType::AntiDiagonal);
    }

    default:
        return Iterator(*this, R, 0, TraversalType::Row);
    }
}

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
    {
        const long len = static_cast<long>(std::min(numRows(), numCols()));
        return ConstIterator(*this, len, len, TraversalType::Diagonal);
    }
    case TraversalType::AntiDiagonal:
    {
        const long R   = static_cast<long>(numRows());
        const long C   = static_cast<long>(numCols());
        const long len = std::min(R, C);
        // match non-const end(): post-increment of the last element lands here
        return ConstIterator(*this, len, C - len - 1, TraversalType::AntiDiagonal);
    }
    default:
        return ConstIterator(*this, static_cast<long>(numRows()), 0, TraversalType::Row);
    }
}


// ======================= Core =======================

template<typename T>
Matrix<T>::Matrix(std::uint32_t numRows, std::uint32_t numCols)
    : rows(numRows), cols(numCols)
{
    data.resize((std::size_t)numRows* (std::size_t)numCols);
}

template<class T>
Matrix<T>::Matrix(Matrix<T>&& other) noexcept
    : data(std::move(other.data)),  // <-- rename to your vector member
      rows(other.rows),
      cols(other.cols)
{
    other.rows = other.cols = 0;
    other.data.clear();             // optional: make moved-from visibly empty
}

template<class T>
Matrix<T>& Matrix<T>::operator=(Matrix<T>&& other) noexcept {
    if (this != &other) {
        data = std::move(other.data);   // <-- rename to your vector member
        rows = other.rows;
        cols = other.cols;
        other.rows = other.cols = 0;
        other.data.clear();             // optional
    }
    return *this;
}


template<typename T>
std::uint32_t Matrix<T>::numRows() const
{
    return rows;
}

template<typename T>
std::uint32_t Matrix<T>::numCols() const
{
    return cols;
}

template<typename T>
RowView<T> Matrix<T>::operator[](std::uint32_t row)
{
    return RowView<T>(data.data()+ toIdx(row, 0), cols);
}

template<typename T>
RowView<const T> Matrix<T>::operator[](std::uint32_t row) const
{
    return RowView<const T>(data.data()+ toIdx(row, 0), cols);
}

template<typename T>
void Matrix<T>::initFromByteStream(const std::uint8_t* byteStream, size_t byteSize)
{
    if (byteSize != (rows * cols)*sizeof(T))throw std::runtime_error("Invalid byte stream size");

    std::memcpy(data.data(), byteStream, byteSize);
}
template<typename T>
void Matrix<T>::initFromByteStream(const std::vector<std::uint8_t>& bytes)
{
    initFromByteStream(bytes.data(), bytes.size());
}



template<typename T>
std::vector<uint8_t> Matrix<T>::toByteStream() const
{
    std::vector<uint8_t> byteStream(rows * cols * sizeof(T));


    std::memcpy(byteStream.data(), data.data(), data.size() * sizeof(T));

    return byteStream;
}

template<>
void Matrix<std::string>::initFromByteStream(const uint8_t* byteStream, size_t byteSize)
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
    size_t counter = 0;
    for (const auto& [row, col, _ ] : *this)
    {
        char* currentCStr = new char [startLocations[counter+1] - startLocations[counter]];
        std::memcpy(currentCStr, byteStream + startLocations[counter], startLocations[counter+1] - startLocations[counter]);
        try
        {
            (*this)[row][col] = std::string(currentCStr);
        }
        catch (const std::exception& e)
        {
            delete[] currentCStr;
            throw;
        }
        delete[] currentCStr;
        counter++;
    }

}

template<>
std::vector<uint8_t> Matrix<std::string>::toByteStream() const
{
    std::vector<uint8_t> result;


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
Matrix<T> Matrix<T>::copy(std::uint32_t startRow,
                          std::uint32_t startCol,
                          int endRow, int endCol) const
{
    const std::uint32_t rEnd =
        (endRow == -1) ? rows :
        (endRow < 0)   ? throw std::out_of_range("endRow < -1"), 0u
        : static_cast<std::uint32_t>(endRow);

    const std::uint32_t cEnd =
        (endCol == -1) ? cols :
        (endCol < 0)   ? throw std::out_of_range("endCol < -1"), 0u
        : static_cast<std::uint32_t>(endCol);

    if (startRow > rows || rEnd > rows) throw std::out_of_range("row range");
    if (startCol > cols || cEnd > cols) throw std::out_of_range("col range");
    if (startRow > rEnd)                throw std::out_of_range("startRow > endRow");
    if (startCol > cEnd)                throw std::out_of_range("startCol > endCol");

    Matrix<T> out(rEnd - startRow, cEnd - startCol);
    for (std::uint32_t i = startRow, r = 0; i < rEnd; ++i, ++r)
        for (std::uint32_t j = startCol, c = 0; j < cEnd; ++j, ++c)
            out[r][c] = (*this)[i][j];
    return out;
}


template<typename T>
void Matrix<T>::paste(const Matrix& src, std::uint32_t destRow, std::uint32_t destCol)
{
    const std::uint32_t h = src.numRows();
    const std::uint32_t w = src.numCols();

    if (h == 0 || w == 0) return; // nothing to do

    // Bounds without overflow:
    if (destRow > rows || destCol > cols)
        throw std::out_of_range("Invalid paste start");
    if (h > rows - destRow || w > cols - destCol)
        throw std::out_of_range("Invalid paste extent");

    // Guard self-paste (same object) — make a temp copy once
    if (&src == this)
    {
        Matrix<T> tmp = src; // cheap for small, correct for all
        for (std::uint32_t i = 0; i < h; ++i)
            for (std::uint32_t j = 0; j < w; ++j)
                (*this)[destRow + i][destCol + j] = tmp[i][j];
        return;
    }

    for (std::uint32_t i = 0; i < h; ++i)
        for (std::uint32_t j = 0; j < w; ++j)
            (*this)[destRow + i][destCol + j] = src[i][j];
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
    for (std::uint32_t i = 0; i < rows; ++i)
        for (std::uint32_t j = 0; j < cols; ++j)
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
std::vector<T> Matrix<T>::getRow(std::uint32_t row, int startCol, int endCol) const
{
    if (row >= rows) throw std::out_of_range("Invalid row index");

    if (endCol == -1) endCol = static_cast<int>(cols);
    if (startCol < 0 || endCol < 0) throw std::out_of_range("Negative column index");

    const std::uint32_t s = static_cast<std::uint32_t>(startCol);
    const std::uint32_t e = static_cast<std::uint32_t>(endCol);
    if (s > e || e > cols) throw std::out_of_range("Invalid column slice indices");

    const T* first = data.data() + toIdx(row, s);
    const T* last  = data.data() + toIdx(row, e);
    return std::vector<T>(first, last);
}

template<>
std::vector<bool>
Matrix<bool>::getRow(std::uint32_t row, int startCol, int endCol) const
{
    if (row >= rows)
        throw std::out_of_range("Invalid row index");

    if (endCol == -1) endCol = static_cast<int>(cols);
    if (startCol < 0 || endCol < 0)
        throw std::out_of_range("Negative column index");

    const std::uint32_t s = static_cast<std::uint32_t>(startCol);
    const std::uint32_t e = static_cast<std::uint32_t>(endCol);
    if (s > e || e > cols)
        throw std::out_of_range("Invalid column slice indices");

    std::vector<bool> out;
    out.reserve(static_cast<std::size_t>(e - s));

    for (std::uint32_t j = s; j < e; ++j)
        out.push_back(data[toIdx(row, j)] != 0);

    return out;
}



template<typename T>
std::vector<T> Matrix<T>::getColumn(std::uint32_t col, int startRow, int endRow) const
{
    if (col >= cols)
        throw std::out_of_range("Invalid column index");

    if (endRow == -1) endRow = static_cast<int>(rows);

    if (startRow < 0 || endRow < 0)
        throw std::out_of_range("Negative row index");

    const std::uint32_t s = static_cast<std::uint32_t>(startRow);
    const std::uint32_t e = static_cast<std::uint32_t>(endRow);

    if (s > e || e > static_cast<std::uint32_t>(rows))
        throw std::out_of_range("Invalid row slice indices");

    std::vector<T> result;
    result.reserve(static_cast<size_t>(e - s));

    for (std::uint32_t i = s; i < e; ++i)
        result.push_back(data[toIdx(i, col)]);

    return result;
}


template<typename T>
std::vector<T> Matrix<T>::getDiagonal(int start, int end) const
{
    if (start < 0) start = 0;
    const std::uint32_t limit = std::min(rows, cols);
    if (end == -1) end = static_cast<int>(limit);
    if (end < 0) throw std::out_of_range("Negative end index");

    const std::uint32_t s = static_cast<std::uint32_t>(start);
    const std::uint32_t e = static_cast<std::uint32_t>(end);

    if (s > e || e > limit)
        throw std::out_of_range("Invalid diagonal indices");

    std::vector<T> result;
    result.reserve(static_cast<size_t>(e - s));

    for (std::uint32_t i = s; i < e; ++i)
        result.push_back(data[toIdx(i, i)]);

    return result;
}


template<typename T>
std::vector<T> Matrix<T>::getAntiDiagonal(int start, int end) const
{
    if (start < 0) start = 0;

    const std::uint32_t limit = std::min(rows, cols);
    if (end == -1) end = static_cast<int>(limit);
    if (end < 0) throw std::out_of_range("Negative end index");

    const std::uint32_t s = static_cast<std::uint32_t>(start);
    const std::uint32_t e = static_cast<std::uint32_t>(end);

    if (s > e || e > limit)
        throw std::out_of_range("Invalid anti-diagonal indices");

    // Empty matrix or zero-width => empty result
    if (cols == 0 || rows == 0 || s == e)
        return {};

    std::vector<T> result;
    result.reserve(static_cast<size_t>(e - s));

    for (std::uint32_t i = s; i < e; ++i)
    {
        const std::uint32_t j = static_cast<std::uint32_t>(cols) - 1u - i;
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
    for (std::uint32_t i = 0; i < lhs.numRows(); ++i)
        for (std::uint32_t j = 0; j < lhs.numCols(); ++j)
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
    for (std::uint32_t i = 0; i < lhs.numRows(); ++i)
        for (std::uint32_t j = 0; j < lhs.numCols(); ++j)
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
    if (data.empty())
        throw std::runtime_error("Matrix::min() on empty matrix");
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
    if (data.empty())
        throw std::runtime_error("Matrix::min() on empty matrix");
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
    std::pair<std::uint32_t, std::uint32_t>>
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
    std::pair<std::uint32_t, std::uint32_t>>
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


// ---------- (1) Compound assignment: Matrix ⊕= Matrix ----------
template<typename T> template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix<T>&>
Matrix<T>::operator+=(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match.");
    for(size_t i = 0; i < data.size(); i++)
    {
        data[i] += other.data[i];
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix<T>&>
Matrix<T>::operator-=(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match.");
    for(size_t i = 0; i < data.size(); i++)
    {
        data[i] -= other.data[i];
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix<T>&>
Matrix<T>::operator*=(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match.");
    for(size_t i = 0; i < data.size(); i++)
    {
        data[i] *= other.data[i];
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix<T>&>
Matrix<T>::operator/=(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match.");
    for(size_t i = 0; i < data.size(); i++)
    {
        if (other.data[i] == 0) throw std::runtime_error("Division by zero encountered.");
        data[i] /= other.data[i];
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator%=(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match.");
    for(size_t i = 0; i < data.size(); i++)
    {
        if (other.data[i] == 0) throw std::runtime_error("Modulus by zero encountered.");
        data[i] %= other.data[i];
    }
    return *this;
}

// ---------- (1) Compound assignment: Matrix ⊕= scalar ----------
template<typename T> template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix<T>&>
Matrix<T>::operator+=(const T& s)
{
    for(size_t i = 0; i < data.size(); i++)
    {
        data[i] += s;
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix<T>&>
Matrix<T>::operator-=(const T& s)
{
    for(size_t i = 0; i < data.size(); i++)
    {
        data[i] -= s;
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix<T>&>
Matrix<T>::operator*=(const T& s)
{
    for(size_t i = 0; i < data.size(); i++)
    {
        data[i] *= s;
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix<T>&>
Matrix<T>::operator/=(const T& s)
{
    if (s == 0)
        throw std::runtime_error("Division by zero encountered.");
    for(size_t i = 0; i < data.size(); i++)
    {
        data[i] /= s;
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator%=(const T& s)
{
    if (s == 0)
        throw std::runtime_error("Modulus by zero encountered.");
    for(size_t i = 0; i < data.size(); i++)
    {
        data[i] %= s;
    }
    return *this;
}

// ---------- Bitwise element-wise (Matrix ⊗ Matrix) ----------
template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator&(const Matrix& other) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] & other.data[i];
    }
    return result;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator|(const Matrix& other) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] | other.data[i];
    }
    return result;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator^(const Matrix& other) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] ^ other.data[i];
    }
    return result;
}


// ---------- Bitwise element-wise (Matrix ⊗ scalar) ----------
template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator&(const T& s) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] & s;
    }
    return result;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator|(const T& s) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] | s;
    }
    return result;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator^(const T& s) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] ^ s;
    }
    return result;
}

// ---------- Compound bitwise (your decls return Matrix by value) ----------
template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator&=(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match.");

    for (size_t i = 0; i < data.size(); ++i)
        data[i] &= other.data[i];

    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator|=(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match.");

    for (size_t i = 0; i < data.size(); ++i)
        data[i] |= other.data[i];

    return *this;
}


template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator^=(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match.");

    for (size_t i = 0; i < data.size(); ++i)
        data[i] ^= other.data[i];

    return *this;
}


template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator&=(const T& s)
{


    for (size_t i = 0; i < data.size(); ++i)
        data[i] &= s;

    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator|=(const T& s)
{


    for (size_t i = 0; i < data.size(); ++i)
        data[i] |= s;

    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator^=(const T& s)
{


    for (size_t i = 0; i < data.size(); ++i)
        data[i] ^= s;

    return *this;
}

// ---------- Unary bitwise NOT ----------
template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator~() const
{
    Matrix<T> result(rows, cols);
    for (size_t i = 0; i < data.size(); ++i)
    {
        result.data[i] = static_cast<T>(~data[i]);
    }
    return result;
}

// ---------- Shifts (integrals only) ----------
#include <limits>

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator<<(int k) const
{
    using Uns = std::make_unsigned_t<U>;
    Matrix<T> out(rows, cols);

    if (data.empty()) return out;
    if (k == 0)
    {
        out.data = data;
        return out;
    }

    const unsigned w = static_cast<unsigned>(std::numeric_limits<Uns>::digits);

    if (k < 0)
    {
        const unsigned s = static_cast<unsigned>(-k) % w;
        for (std::size_t i = 0; i < data.size(); ++i)
        {
            Uns u = static_cast<Uns>(data[i]);
            u >>= s; // logical right shift
            out.data[i] = static_cast<T>(u);
        }
        return out;
    }

    const unsigned s = static_cast<unsigned>(k) % w;
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        Uns u = static_cast<Uns>(data[i]);
        u <<= s; // logical left shift on Uns
        out.data[i] = static_cast<T>(u);
    }
    return out;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>>
        Matrix<T>::operator>>(int k) const
{
    using Uns = std::make_unsigned_t<U>;
    using Sig = std::make_signed_t<U>;

    Matrix<T> out(rows, cols);
    if (data.empty()) return out;
    if (k == 0)
    {
        out.data = data;
        return out;
    }

    const unsigned w = static_cast<unsigned>(std::numeric_limits<Uns>::digits);

    if (k < 0)
    {
        const unsigned s = static_cast<unsigned>(-k) % w;
        // negative k => left shift
        for (std::size_t i = 0; i < data.size(); ++i)
        {
            Uns u = static_cast<Uns>(data[i]);
            u <<= s;
            out.data[i] = static_cast<T>(u);
        }
        return out;
    }

    const unsigned s = static_cast<unsigned>(k) % w;
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        if constexpr (std::is_signed<U>::value)
        {
            Sig v = static_cast<Sig>(data[i]);
            v >>= s; // arithmetic right shift
            out.data[i] = static_cast<T>(v);
        }
        else
        {
            Uns u = static_cast<Uns>(data[i]);
            u >>= s; // logical right shift
            out.data[i] = static_cast<T>(u);
        }
    }
    return out;
}
template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator<<=(int k)
{
    using Uns = std::make_unsigned_t<U>;
    if (data.empty() || k == 0) return *this;

    const unsigned w = static_cast<unsigned>(std::numeric_limits<Uns>::digits);

    if (k < 0)
    {
        const unsigned s = static_cast<unsigned>(-k) % w;
        for (std::size_t i = 0; i < data.size(); ++i)
        {
            Uns u = static_cast<Uns>(data[i]);
            u >>= s;
            data[i] = static_cast<T>(u);
        }
        return *this;
    }

    const unsigned s = static_cast<unsigned>(k) % w;
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        Uns u = static_cast<Uns>(data[i]);
        u <<= s;
        data[i] = static_cast<T>(u);
    }
    return *this;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_math_integral<U>::value, Matrix<T>&>
Matrix<T>::operator>>=(int k)
{
    using Uns = std::make_unsigned_t<U>;
    using Sig = std::make_signed_t<U>;

    if (data.empty() || k == 0) return *this;

    const unsigned w = static_cast<unsigned>(std::numeric_limits<Uns>::digits);

    if (k < 0)
    {
        const unsigned s = static_cast<unsigned>(-k) % w;
        // negative k => left shift
        for (std::size_t i = 0; i < data.size(); ++i)
        {
            Uns u = static_cast<Uns>(data[i]);
            u <<= s;
            data[i] = static_cast<T>(u);
        }
        return *this;
    }

    const unsigned s = static_cast<unsigned>(k) % w;
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        if constexpr (std::is_signed<U>::value)
        {
            Sig v = static_cast<Sig>(data[i]);
            v >>= s; // arithmetic
            data[i] = static_cast<T>(v);
        }
        else
        {
            Uns u = static_cast<Uns>(data[i]);
            u >>= s; // logical
            data[i] = static_cast<T>(u);
        }
    }
    return *this;
}



// ---------- Logical ops for Matrix<bool> ----------
template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, Matrix<T>>
        Matrix<T>::logical_and(const Matrix& other) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] && other.data[i];
    }
    return result;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, Matrix<T>>
        Matrix<T>::logical_or(const Matrix& other) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] || other.data[i];
    }
    return result;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, Matrix<T>>
        Matrix<T>::logical_xor(const Matrix& other) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] != other.data[i];
    }
    return result;
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, Matrix<T>>
        Matrix<T>::logical_not() const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = !data[i];
    }
    return result;
}

// scalar versions (bool on the right)
template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, Matrix<T>>
        Matrix<T>::logical_and(bool s) const
{
    {
        Matrix<T> result(rows, cols);
        for(size_t i = 0; i < data.size(); i++)
        {
            result.data[i] = data[i] && s;
        }
        return result;
    }
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, Matrix<T>>
        Matrix<T>::logical_or(bool s) const

{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] || s;
    }
    return result;
}


template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, Matrix<T>>
        Matrix<T>::logical_xor(bool s) const
{
    Matrix<T> result(rows, cols);
    for(size_t i = 0; i < data.size(); i++)
    {
        result.data[i] = data[i] != s;
    }
    return result;
}
// reductions/convenience for masks
template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, std::size_t>
Matrix<T>::count_true() const noexcept
{
    size_t result = 0;
    for(size_t i = 0; i < data.size(); i++)if(data[i])result++;
    return result;
}
template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, bool>
Matrix<T>::any() const noexcept
{
    for (bool cell : data)            // data stores uint8_t for bool
        if (cell) return true;        // first true => done
    return false;                   // empty matrix also returns false
}

template<typename T> template<typename U>
std::enable_if_t<bml_is_bool<U>::value, bool>
Matrix<T>::none() const noexcept
{
    for (bool cell : data)
        if (cell) return false;       // first true => not none
    return true;                    // empty matrix returns true
}



