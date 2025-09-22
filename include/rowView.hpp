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
    RowView(T* dataPointer, std::uint32_t length) noexcept;

    // ---- element access (bounds-checked) ----
    // Throws std::out_of_range if col >= length.
    T&       operator[](std::uint32_t col);
    const T& operator[](std::uint32_t col) const;

    // ---- iteration (enables range-for) ----
    T*       begin() noexcept;
    T*       end()   noexcept;
    const T* begin() const noexcept;
    const T* end()   const noexcept;

    // ---- info / raw access ----
    std::uint32_t size()  const noexcept;
    bool         empty() const noexcept;
    T*           data()  const noexcept;



private:
    T*           row;
    std::uint32_t length;
};
// ---------- Writable proxy for byte-backed bools (declaration only) ----------
class BoolRef {
public:
    explicit BoolRef(std::uint8_t* p) noexcept;

    // READ
    operator bool() const noexcept;

    // WRITE
    BoolRef& operator=(bool v) noexcept;
    BoolRef& operator=(const BoolRef& o) noexcept;

    // Nice-to-have ops
    BoolRef& operator&=(bool v) noexcept;
    BoolRef& operator|=(bool v) noexcept;
    BoolRef& operator^=(bool v) noexcept;

    friend bool operator==(BoolRef a, bool b) noexcept { return static_cast<bool>(a) == b; }
    friend bool operator!=(BoolRef a, bool b) noexcept { return !(a == b); }

private:
    std::uint8_t* p_;
};

// ====================== specialisation: RowView<bool> (declaration) ======================
template<> class RowView<bool>
{
public:
    RowView() noexcept;
    RowView(std::uint8_t* dataPointer, std::uint32_t length) noexcept;

    // element access (bounds-checked)
    BoolRef operator[](std::uint32_t col);
    bool    operator[](std::uint32_t col) const;

    // iteration (range-for)
    struct iterator;        // defined in .cpp
    struct const_iterator;  // defined in .cpp
    iterator       begin() noexcept;
    iterator       end()   noexcept;
    const_iterator begin() const noexcept;
    const_iterator end()   const noexcept;

    // info / raw access
    std::uint32_t size()  const noexcept;
    bool         empty() const noexcept;

    // bool has no meaningful bool*; expose storage pointer instead
    std::uint8_t*       data_storage()       noexcept;
    const std::uint8_t* data_storage() const noexcept;

    // block T* API for bool
    bool* data() const noexcept = delete;


private:
    std::uint8_t* row;
    std::uint32_t  length;
};

// ================== specialisation: RowView<const bool> (declaration) ==================
template<> class RowView<const bool>
{
public:
    RowView() noexcept;
    RowView(const std::uint8_t* dataPointer, std::uint32_t length) noexcept;

    bool operator[](std::uint32_t col) const;

    struct const_iterator; // defined in .cpp
    const_iterator begin() const noexcept;
    const_iterator end()   const noexcept;

    std::uint32_t size()  const noexcept;
    bool         empty() const noexcept;

    const std::uint8_t* data_storage() const noexcept;

    const bool* data() const noexcept = delete;


private:
    const std::uint8_t* row;
    std::uint32_t        length;
};

#endif // ROWVIEW_HPP
