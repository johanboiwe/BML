#ifndef BML_BOOLREF_HPP
#define BML_BOOLREF_HPP

#include <cstdint>
#include <iostream>
#include "export.hpp"

/**
 * @file boolRef.hpp
 * @brief Non-owning proxy for a single boolean cell in a Matrix<bool>.
 *
 * BML stores Matrix<bool> as bytes (uint8_t) instead of C++ bool, to avoid
 * bit-packing and other surprises. BoolRef acts like a reference to one of
 * those bytes:
 *
 * - You can read it in a boolean context:
 *   @code
 *   BoolRef br = row[3];
 *   if (br) { ... }
 *   @endcode
 *
 * - You can assign to it:
 *   @code
 *   br = true;
 *   br &= false;
 *   @endcode
 *
 * - It does NOT own the memory. If the parent Matrix<bool> is destroyed,
 *   any BoolRef pointing into it becomes invalid.
 *
 * Users normally don't create BoolRef directly; it comes from
 * Matrix<bool>::operator[] / iterators.
 */

namespace bml
{
    class BML_API BoolRef {
    public:
        /**
         * @brief Construct a proxy for a byte-backed bool.
         * @param p Pointer to the byte holding the boolean value (0 or non-zero).
         * @pre p != nullptr
         */
        explicit BoolRef(std::uint8_t* p) noexcept;

        /**
         * @brief Copy-construct from another proxy (aliases the same storage).
         *
         * After this, both BoolRef objects still refer to the same underlying byte.
         * No deep copy is made.
         */
        BoolRef(const BoolRef& other) = default;

        /**
         * @brief Implicit conversion to bool (read).
         * @return true if the referenced byte is non-zero; otherwise false.
         */
        explicit operator bool() const noexcept;

        /**
         * @brief Assign from a bool (write).
         * @param v New value; stored as 0 or 1.
         * @return *this
         */
        BoolRef& operator=(bool v) noexcept;

        /**
         * @brief Assign from another proxy (read then write).
         * @param o Source proxy.
         * @return *this
         */
        BoolRef& operator=(const BoolRef& o) noexcept;

        /**
         * @brief Bitwise-AND assign with a bool.
         * @param v Value to AND in.
         * @return *this
         */
        BoolRef& operator&=(bool v) noexcept;

        /**
         * @brief Bitwise-OR assign with a bool.
         * @param v Value to OR in.
         * @return *this
         */
        BoolRef& operator|=(bool v) noexcept;

        /**
         * @brief Bitwise-XOR assign with a bool.
         * @param v Value to XOR in.
         * @return *this
         */
        BoolRef& operator^=(bool v) noexcept;

    private:
        /// Pointer to the referenced storage byte (non-owning). Guaranteed non-null after construction.
        std::uint8_t* p_;
    }; // class BoolRef


    // ---------- Free comparison & stream operators ----------

    /**
     * @brief Equality between two BoolRef proxies.
     * @param a Left-hand proxy.
     * @param b Right-hand proxy.
     * @return true iff both read the same boolean value.
     */
    BML_API bool operator==(const BoolRef& a, const BoolRef& b) noexcept;

    /**
     * @brief Inequality between two BoolRef proxies.
     * @param a Left-hand proxy.
     * @param b Right-hand proxy.
     * @return true iff the read boolean values differ.
     */
    BML_API bool operator!=(const BoolRef& a, const BoolRef& b) noexcept;

    /**
     * @brief Equality between a BoolRef and a bool (proxy on the left).
     * @param a Left-hand BoolRef proxy.
     * @param b Right-hand bool value.
     * @return true iff @p a reads the same value as @p b.
     */
    BML_API bool operator==(const BoolRef& a, bool b) noexcept;

    /**
     * @brief Inequality between a BoolRef and a bool (proxy on the left).
     * @param a Left-hand BoolRef proxy.
     * @param b Right-hand bool value.
     * @return true iff @p a and @p b differ.
     */
    BML_API bool operator!=(const BoolRef& a, bool b) noexcept;

    /**
     * @brief Equality between a bool and a BoolRef (proxy on the right).
     * @param a Left-hand bool value.
     * @param b Right-hand BoolRef proxy.
     * @return true iff @p a equals the value read from @p b.
     *
     * This lets you write code like:
     * @code
     * if (true == matrixBool[r][c]) { ... }
     * @endcode
     */
    BML_API bool operator==(bool a, const BoolRef& b) noexcept;

    /**
     * @brief Inequality between a bool and a BoolRef (proxy on the right).
     * @param a Left-hand bool value.
     * @param b Right-hand BoolRef proxy.
     * @return true iff @p a and @p b differ.
     */
    BML_API  bool operator!=(bool a, const BoolRef& b) noexcept;

    /**
     * @brief Stream output of the referenced boolean value.
     * @param os Output stream.
     * @param br Proxy to print.
     * @return os
     *
     * Prints `true` or `false` just like a normal bool.
     */
    BML_API std::ostream& operator<<(std::ostream& os, BoolRef br);

} // namespace bml

#endif // BML_BOOLREF_HPP
