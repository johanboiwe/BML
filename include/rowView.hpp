#ifndef ROWVIEW_HPP
#define ROWVIEW_HPP
#include <vector>
#include <cstdint>
/**
 * @brief Writable proxy to a byte-backed boolean.
 *
 * Models an lvalue-like reference to a bool stored in a single @c std::uint8_t.
 * Reads convert any non-zero to @c true; writes store @c 0 or @c 1.
 *
 * @note Does not own memory; the caller guarantees @p p_ remains valid.
 * @warning This wraps a whole byte. If you intend bit-packing, you’ll want a
 *          pointer + bit index/mask rather than a single pointer.
 */
class BoolRef {
public:
    /**
     * @brief Construct a proxy for a byte-backed bool.
     * @param p Pointer to the byte holding the boolean value (0 or non-zero).
     * @pre p != nullptr
     */
    explicit BoolRef(std::uint8_t* p) noexcept;

    /**
     * @brief Copy-construct from another proxy (aliases the same storage).
     */
    BoolRef(const BoolRef& other) noexcept;

    /**
     * @brief Implicit conversion to bool (read).
     * @return @c true if the referenced byte is non-zero; otherwise @c false.
     */
    operator bool() const noexcept;

    /**
     * @brief Assign from a bool (write).
     * @param v New value; stored as 0 or 1.
     * @return *this
     */
    BoolRef& operator=(bool v) noexcept;

    /**
     * @brief Assign from another proxy (read then write).
     * @return *this
     */
    BoolRef& operator=(const BoolRef& o) noexcept;

    /**
     * @brief Bitwise-AND assign with a bool.
     * @return *this
     */
    BoolRef& operator&=(bool v) noexcept;

    /**
     * @brief Bitwise-OR assign with a bool.
     * @return *this
     */
    BoolRef& operator|=(bool v) noexcept;

    /**
     * @brief Bitwise-XOR assign with a bool.
     * @return *this
     */
    BoolRef& operator^=(bool v) noexcept;

    /// Compare to a bool by value.
    friend bool operator==(BoolRef& a, bool b) noexcept { return static_cast<bool>(a) == b; }
    /// Negation of equality.
    friend bool operator!=(BoolRef& a, bool b) noexcept { return !(a == b); }

private:
    std::uint8_t* p_; ///< Pointer to the referenced storage byte (non-owning).
};


/**
 * @brief Lightweight, non-owning view of a single matrix row.
 * @tparam T Element type (e.g., bool, std::int8_t, std::uint8_t, std::int16_t,
 *           std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
 *           float, double, long double, char16_t, char32_t, std::string, void*).
 * @note  The view does not own memory; the underlying storage must remain valid.
 */
template<typename T>
class RowView
{
public:
    /// @brief Construct an empty row view (size()==0, data()==nullptr).
    RowView() noexcept;

    /**
     * @brief Construct a view over a contiguous row.
     * @param[in] dataPointer Pointer to the first element of the row (may be nullptr iff length==0).
     * @param[in] length      Number of elements addressable via this view.
     * @pre dataPointer is valid for @p length elements (if @p length > 0).
     */
    RowView(T* dataPointer, std::uint32_t length) noexcept;

    /**
     * @brief Random access.
     * @param[in] col Column index in [0, size()).
     * @return Reference to the element at @p col.
     * @pre col < size().
     */
    T&       operator[](std::uint32_t col);
    /// @copydoc operator[](std::uint32_t)
    const T& operator[](std::uint32_t col) const;

    /// @brief Begin iterator (mutable).
    T*       begin() noexcept;
    /// @brief End iterator (mutable).
    T*       end()   noexcept;

    /// @brief Begin iterator (const).
    const T* begin() const noexcept;
    /// @brief End iterator (const).
    const T* end()   const noexcept;

    /// @brief Number of elements in the row.
    std::uint32_t size()  const noexcept;

    /// @brief True if size()==0.
    bool          empty() const noexcept;

    /// @brief Pointer to the first element (may be nullptr when empty).
    const T*      data()  const noexcept;

private:
    T*            row;      ///< Pointer to first element (non-owning).
    std::uint32_t length;   ///< Element count addressable via this view.
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
