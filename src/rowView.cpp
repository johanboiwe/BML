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



// ---------- BoolRef definitions ----------
BoolRef::BoolRef(std::uint8_t* p) noexcept : p_(p) {}

BoolRef::operator bool() const noexcept { return *p_ != 0; }

BoolRef& BoolRef::operator=(bool v) noexcept {
    *p_ = static_cast<std::uint8_t>(v ? 1 : 0);
    return *this;
}

BoolRef& BoolRef::operator=(const BoolRef& other) noexcept {
    return (*this = static_cast<bool>(other));
}

// ====================== RowView<bool> definitions ======================
RowView<bool>::RowView() noexcept : row(nullptr), length(0) {}

RowView<bool>::RowView(std::uint8_t* dataPointer, unsigned int length) noexcept
    : row(dataPointer), length(length) {}

BoolRef RowView<bool>::operator[](unsigned int col) {
    if (col >= length) throw std::out_of_range("RowView<bool>: col out of range");
    return BoolRef{ row + col };
}

bool RowView<bool>::operator[](unsigned int col) const {
    if (col >= length) throw std::out_of_range("RowView<bool>: col out of range");
    return row[col] != 0;
}

// iterators (index-based)
struct RowView<bool>::iterator {
    std::uint8_t* p; unsigned int i;
    BoolRef operator*() const { return BoolRef{ p + i }; }
    iterator& operator++() { ++i; return *this; }
    bool operator!=(const iterator& o) const { return i != o.i; }
};

struct RowView<bool>::const_iterator {
    const std::uint8_t* p; unsigned int i;
    bool operator*() const { return p[i] != 0; }
    const_iterator& operator++() { ++i; return *this; }
    bool operator!=(const const_iterator& o) const { return i != o.i; }
};

RowView<bool>::iterator RowView<bool>::begin() noexcept       { return iterator{ row, 0u }; }
RowView<bool>::iterator RowView<bool>::end()   noexcept       { return iterator{ row, length }; }
RowView<bool>::const_iterator RowView<bool>::begin() const noexcept { return const_iterator{ row, 0u }; }
RowView<bool>::const_iterator RowView<bool>::end()   const noexcept { return const_iterator{ row, length }; }

unsigned int RowView<bool>::size() const noexcept  { return length; }
bool RowView<bool>::empty() const noexcept         { return length == 0; }

std::uint8_t* RowView<bool>::data_storage() noexcept             { return row; }
const std::uint8_t* RowView<bool>::data_storage() const noexcept { return row; }


// =================== RowView<const bool> definitions ===================
RowView<const bool>::RowView() noexcept : row(nullptr), length(0) {}

RowView<const bool>::RowView(const std::uint8_t* dataPointer, unsigned int length) noexcept
    : row(dataPointer), length(length) {}

bool RowView<const bool>::operator[](unsigned int col) const {
    if (col >= length) throw std::out_of_range("RowView<const bool>: col out of range");
    return row[col] != 0;
}

struct RowView<const bool>::const_iterator {
    const std::uint8_t* p; unsigned int i;
    bool operator*() const { return p[i] != 0; }
    const_iterator& operator++() { ++i; return *this; }
    bool operator!=(const const_iterator& o) const { return i != o.i; }
};

RowView<const bool>::const_iterator RowView<const bool>::begin() const noexcept { return const_iterator{ row, 0u }; }
RowView<const bool>::const_iterator RowView<const bool>::end()   const noexcept { return const_iterator{ row, length }; }

unsigned int RowView<const bool>::size() const noexcept  { return length; }
bool RowView<const bool>::empty() const noexcept         { return length == 0; }

const std::uint8_t* RowView<const bool>::data_storage() const noexcept { return row; }


