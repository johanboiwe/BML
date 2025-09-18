#ifndef ROWVIEW_HPP
#define ROWVIEW_HPP
#include <vector>

template<typename T>
class RowView
{
public:
    // ---- constructors ----
    RowView() noexcept;
    RowView(T* dataPointer, unsigned int length) noexcept;

    // ---- element access (bounds-checked) ----
    // Throws std::out_of_range if col >= length.
    T&       operator[](unsigned int col);
    const T& operator[](unsigned int col) const;

    // ---- iteration (enables range-for) ----
    T*       begin() noexcept;
    T*       end()   noexcept;
    const T* begin() const noexcept;
    const T* end()   const noexcept;

    // ---- info / raw access ----
    unsigned int size()  const noexcept;
    bool         empty() const noexcept;
    T*           data()  const noexcept;

    std::vector<T> rowAsVector() const;

private:
    T*           row;
    unsigned int length;
};


#endif // ROWVIEW_HPP
