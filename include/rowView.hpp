#ifndef ROWVIEW_HPP
#define ROWVIEW_HPP
#include <vector>
#include <cstdint>

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



private:
    T*           row;
    unsigned int length;
};
// ---------- Writable proxy for byte-backed bools (declaration only) ----------
class BoolRef
{
public:
    explicit BoolRef(std::uint8_t* p) noexcept;
    operator bool() const noexcept;
    BoolRef& operator=(bool v) noexcept;
    BoolRef& operator=(const BoolRef& other) noexcept;

private:
    std::uint8_t* p_;
};

// ====================== specialisation: RowView<bool> (declaration) ======================
template<> class RowView<bool>
{
public:
    RowView() noexcept;
    RowView(std::uint8_t* dataPointer, unsigned int length) noexcept;

    // element access (bounds-checked)
    BoolRef operator[](unsigned int col);
    bool    operator[](unsigned int col) const;

    // iteration (range-for)
    struct iterator;        // defined in .cpp
    struct const_iterator;  // defined in .cpp
    iterator       begin() noexcept;
    iterator       end()   noexcept;
    const_iterator begin() const noexcept;
    const_iterator end()   const noexcept;

    // info / raw access
    unsigned int size()  const noexcept;
    bool         empty() const noexcept;

    // bool has no meaningful bool*; expose storage pointer instead
    std::uint8_t*       data_storage()       noexcept;
    const std::uint8_t* data_storage() const noexcept;

    // block T* API for bool
    bool* data() const noexcept = delete;

    std::vector<bool> rowAsVector() const;

private:
    std::uint8_t* row;
    unsigned int  length;
};

// ================== specialisation: RowView<const bool> (declaration) ==================
template<> class RowView<const bool>
{
public:
    RowView() noexcept;
    RowView(const std::uint8_t* dataPointer, unsigned int length) noexcept;

    bool operator[](unsigned int col) const;

    struct const_iterator; // defined in .cpp
    const_iterator begin() const noexcept;
    const_iterator end()   const noexcept;

    unsigned int size()  const noexcept;
    bool         empty() const noexcept;

    const std::uint8_t* data_storage() const noexcept;

    const bool* data() const noexcept = delete;


private:
    const std::uint8_t* row;
    unsigned int        length;
};

#endif // ROWVIEW_HPP
