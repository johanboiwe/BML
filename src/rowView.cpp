#include "rowView.hpp"
#include <stdexcept>   // std::out_of_range
#include <vector>      // std::vector

// ---- constructors ----
template<typename T>
RowView<T>::RowView() noexcept : row(nullptr), length(0) {}

template<typename T>
RowView<T>::RowView(T* dataPointer, unsigned int length_) noexcept
    : row(dataPointer), length(length_) {}

// ---- element access (bounds-checked) ----
template<typename T>
T& RowView<T>::operator[](unsigned int col)
{
    if (col >= length) throw std::out_of_range("RowView::operator[] out of range");
    return row[col];
}

template<typename T>
const T& RowView<T>::operator[](unsigned int col) const
{
    if (col >= length) throw std::out_of_range("RowView::operator[] out of range");
    return row[col];
}

// ---- iteration ----
template<typename T>
T* RowView<T>::begin() noexcept
{
    return row;
}

template<typename T>
T* RowView<T>::end() noexcept
{
    return row + length;
}

template<typename T>
const T* RowView<T>::begin() const noexcept
{
    return row;
}

template<typename T>
const T* RowView<T>::end() const noexcept
{
    return row + length;
}

// ---- info / raw access ----
template<typename T>
unsigned int RowView<T>::size() const noexcept
{
    return length;
}

template<typename T>
bool RowView<T>::empty() const noexcept
{
    return length == 0;
}

template<typename T>
T* RowView<T>::data() const noexcept
{
    return row;
}

// ---- OPTIONAL convenience ----
// If you want this, DECLARE it in the header: `std::vector<T> rowAsVector() const;`
template<typename T>
std::vector<T> RowView<T>::rowAsVector() const
{
    return std::vector<T>(row, row + length); // copies elements
}
