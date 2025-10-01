#include "../include/bml/rowView.hpp"
#include <stdexcept>   // std::out_of_range

#include "bml/boolRef.hpp"

namespace bml
{
    // ---- constructors ----
    template<typename T>
    RowView<T>::RowView() noexcept : row(nullptr), length(0) {}

    template<typename T>
    RowView<T>::RowView(T* dataPointer, std::uint32_t length_) noexcept
        : row(dataPointer), length(length_) {}

    // ---- element access (bounds-checked) ----
    template<typename T>
    T& RowView<T>::operator[](std::uint32_t col)
    {
        if (col >= length) throw std::out_of_range("RowView::operator[] out of range");
        return row[col];
    }

    template<typename T>
    const T& RowView<T>::operator[](std::uint32_t col) const
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
    std::uint32_t RowView<T>::size() const noexcept
    {
        return length;
    }

    template<typename T>
    bool RowView<T>::empty() const noexcept
    {
        return length == 0;
    }

    template<typename T>
    const T* RowView<T>::data() const noexcept
    {
        return row;
    }





    // ====================== RowView<bool> definitions ======================
    RowView<bool>::RowView() noexcept : row(nullptr), length(0) {}

    RowView<bool>::RowView(std::uint8_t* dataPointer, std::uint32_t length) noexcept
        : row(dataPointer), length(length) {}

    BoolRef RowView<bool>::operator[](std::uint32_t col) {
        if (col >= length) throw std::out_of_range("RowView<bool>: col out of range");
        return BoolRef{ row + col };
    }

    bool RowView<bool>::operator[](std::uint32_t col) const {
        if (col >= length) throw std::out_of_range("RowView<bool>: col out of range");
        return row[col] != 0;
    }


    std::uint32_t RowView<bool>::size() const noexcept  { return length; }
    bool RowView<bool>::empty() const noexcept         { return length == 0; }

    std::uint8_t* RowView<bool>::data_storage() noexcept             { return row; }
    const std::uint8_t* RowView<bool>::data_storage() const noexcept { return row; }


    // =================== RowView<const bool> definitions ===================
    RowView<const bool>::RowView() noexcept : row(nullptr), length(0) {}

    RowView<const bool>::RowView(const std::uint8_t* dataPointer, std::uint32_t length) noexcept
        : row(dataPointer), length(length) {}

    bool RowView<const bool>::operator[](std::uint32_t col) const {
        if (col >= length) throw std::out_of_range("RowView<const bool>: col out of range");
        return row[col] != 0;
    }



    std::uint32_t RowView<const bool>::size() const noexcept  { return length; }
    bool RowView<const bool>::empty() const noexcept         { return length == 0; }

    const std::uint8_t* RowView<const bool>::data_storage() const noexcept { return row; }
}
