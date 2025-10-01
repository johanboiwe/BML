#ifndef ROWVIEW_HPP
#define ROWVIEW_HPP

#include <cstdint>
#include "bml/boolRef.hpp"
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

namespace bml
{



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
         * @param[in] dataPointer Pointer to the first element of the row (maybe nullptr iff length==0).
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
        /// \overload
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
        [[nodiscard]] std::uint32_t size()  const noexcept;

        /// @brief True if size()==0.
        [[nodiscard]] bool          empty() const noexcept;

        /// @brief Pointer to the first element (maybe nullptr when empty).
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
        class BoolRowViewIterator;

        BoolRowViewIterator       begin() noexcept;
        BoolRowViewIterator       end()   noexcept;


        // info / raw access
        [[nodiscard]] std::uint32_t size()  const noexcept;
        [[nodiscard]] bool         empty() const noexcept;

        // bool has no meaningful bool*; expose storage pointer instead
        std::uint8_t*       data_storage()       noexcept;
        [[nodiscard]] const std::uint8_t* data_storage() const noexcept;

        // block T* API for bool
        [[nodiscard]] bool* data() const noexcept = delete;


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

        class ConstBoolRowViewIterator; // defined in .cpp
        [[nodiscard]] ConstBoolRowViewIterator begin() const noexcept;
        [[nodiscard]] ConstBoolRowViewIterator end()   const noexcept;

        [[nodiscard]] std::uint32_t size()  const noexcept;
        [[nodiscard]] bool         empty() const noexcept;

        [[nodiscard]] const std::uint8_t* data_storage() const noexcept;

        [[nodiscard]] const bool* data() const noexcept = delete;


    private:
        const std::uint8_t* row;
        std::uint32_t        length;
    };
}
#endif // ROWVIEW_HPP
