#ifndef ROWVIEW_HPP
#define ROWVIEW_HPP

#include <cstdint>
#include "boolRef.hpp"

/**
 * @file bml/rowView.hpp
 * @brief Row-only, non-owning views over matrix storage (generic and bool-specialised).
 *
 * @note Views do not own memory; the backing storage must outlive the view.
 * @see bml::BoolRef for byte-backed bool element proxies.
 */

namespace bml
{

/**
 * @brief Lightweight, non-owning view of a single matrix row.
 * @tparam T Element type (e.g., arithmetic types, char16_t/char32_t, std::string, void*).
 *
 * Provides contiguous, random-access element references into an existing row.
 * No bounds checks are performed unless stated; UB if preconditions are violated.
 */
template<typename T>
class RowView
{
public:
    /// @brief Construct an empty row view (size()==0, data()==nullptr).
    RowView() noexcept;

    /**
     * @brief Construct a view over a contiguous row.
     * @param[in] dataPointer Pointer to the first element (may be nullptr iff length==0).
     * @param[in] length      Number of elements addressable via this view.
     * @pre dataPointer is valid for @p length elements if @p length > 0.
     */
    RowView(T* dataPointer, std::uint32_t length) noexcept;

    /**
     * @brief Random access (unchecked).
     * @param[in] col Column index in [0, size()).
     * @return Reference to element @p col.
     * @pre col < size().
     */
    T&       operator[](std::uint32_t col);
    /// \overload
    const T& operator[](std::uint32_t col) const;

    /// @brief Begin iterator (mutable). Valid while the underlying storage is valid.
    T*       begin() noexcept;
    /// @brief End iterator (mutable).
    T*       end()   noexcept;

    /// @brief Begin iterator (const).
    const T* begin() const noexcept;
    /// @brief End iterator (const).
    const T* end()   const noexcept;

    /// @brief Number of elements in the row.
    [[nodiscard]] std::uint32_t size()  const noexcept;

    /// @brief True if size()==0.
    [[nodiscard]] bool          empty() const noexcept;

    /// @brief Pointer to the first element (may be nullptr when empty).
    const T*      data()  const noexcept;

private:
    T*            row;      ///< Pointer to first element (non-owning).
    std::uint32_t length;   ///< Element count addressable via this view.
};


// ====================== specialisation: RowView<bool> (declaration) ======================

/**
 * @brief Row view for byte-backed @c bool storage.
 *
 * Elements read as @c bool, and write via @ref BoolRef which stores @c 0/@c 1 in a byte.
 * @note No bit-packing; each logical bool occupies one byte in the backing store.
 */
template<> class RowView<bool>
{
public:
    /// @brief Construct an empty row view (size()==0).
    RowView() noexcept;

    /**
     * @brief Construct a bool row view over byte storage.
     * @param[in] dataPointer Pointer to the first byte (may be nullptr iff length==0).
     * @param[in] length      Number of logical bool elements.
     * @pre dataPointer is valid for @p length bytes if @p length > 0.
     */
    RowView(std::uint8_t* dataPointer, std::uint32_t length) noexcept;

    /**
     * @brief Element access (mutable).
     * @param[in] col Column index in [0, size()).
     * @return Writable proxy for the @p col element.
     * @pre col < size().
     */
    BoolRef operator[](std::uint32_t col);

    /**
     * @brief Element access (const).
     * @param[in] col Column index in [0, size()).
     * @return Element value at @p col.
     * @pre col < size().
     */
    bool    operator[](std::uint32_t col) const;

    /**
     * @brief Forward iterator over @ref BoolRef proxies (range-for support).
     *
     * Dereferencing yields @ref BoolRef for mutation.
     * Iterators are invalidated if the underlying storage is invalidated.
     */
    class BoolRowViewIterator;

    /// @brief Iterator to first element (proxy-yielding).
    BoolRowViewIterator begin() noexcept;
    /// @brief Iterator past the last element.
    BoolRowViewIterator end()   noexcept;

    /// @brief Number of elements in the row.
    [[nodiscard]] std::uint32_t size()  const noexcept;

    /// @brief True if size()==0.
    [[nodiscard]] bool          empty() const noexcept;

    /**
     * @brief Access the underlying byte storage pointer.
     * @return Pointer to first byte (may be nullptr when empty).
     *
     * @warning Exposes representation; mutate with care.
     */
    std::uint8_t*             data_storage()       noexcept;
    /// \overload
    [[nodiscard]] const std::uint8_t* data_storage() const noexcept;

    /// @brief Disabled: @c bool* is not meaningful for byte-backed representation.
    [[nodiscard]] bool* data() const noexcept = delete;

private:
    std::uint8_t* row;     ///< Pointer to first byte (non-owning).
    std::uint32_t length;  ///< Number of logical elements.
};


// ================== specialisation: RowView<const bool> (declaration) ==================

/**
 * @brief Const row view for byte-backed @c bool storage.
 *
 * Read-only view; element access yields @c bool values. No mutation possible.
 */
template<> class RowView<const bool>
{
public:
    /// @brief Construct an empty const row view (size()==0).
    RowView() noexcept;

    /**
     * @brief Construct a const bool row view over byte storage.
     * @param[in] dataPointer Pointer to the first byte (may be nullptr iff length==0).
     * @param[in] length      Number of logical bool elements.
     * @pre dataPointer is valid for @p length bytes if @p length > 0.
     */
    RowView(const std::uint8_t* dataPointer, std::uint32_t length) noexcept;

    /**
     * @brief Element access (const).
     * @param[in] col Column index in [0, size()).
     * @return Element value at @p col.
     * @pre col < size().
     */
    bool operator[](std::uint32_t col) const;

    /**
     * @brief Forward iterator over @c bool values (range-for support).
     *
     * Dereferencing yields @c bool by value. Iterator invalidation follows the
     * underlying storage lifetime.
     */
    class ConstBoolRowViewIterator; // defined in .cpp

    /// @brief Iterator to first element.
    [[nodiscard]] ConstBoolRowViewIterator begin() const noexcept;
    /// @brief Iterator past the last element.
    [[nodiscard]] ConstBoolRowViewIterator end()   const noexcept;

    /// @brief Number of elements in the row.
    [[nodiscard]] std::uint32_t size()  const noexcept;

    /// @brief True if size()==0.
    [[nodiscard]] bool          empty() const noexcept;

    /**
     * @brief Access the underlying byte storage pointer.
     * @return Pointer to first byte (may be nullptr when empty).
     */
    [[nodiscard]] const std::uint8_t* data_storage() const noexcept;

    /// @brief Disabled: @c const bool* is not meaningful for byte-backed representation.
    [[nodiscard]] const bool* data() const noexcept = delete;

private:
    const std::uint8_t* row;  ///< Pointer to first byte (non-owning).
    std::uint32_t       length; ///< Length of row.
};

} // namespace bml
#endif // ROWVIEW_HPP
