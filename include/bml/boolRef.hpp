//
// Created by johan on 2025-09-26.
//

#ifndef BML_BOOLREF_HPP
#define BML_BOOLREF_HPP
#include <cstdint>
#include <iostream>

namespace bml
{
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
        BoolRef(const BoolRef& other) = default;

        /**
         * @brief Implicit conversion to bool (read).
         * @return @c true if the referenced byte is non-zero; otherwise @c false.
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

        explicit operator bool() noexcept;

    private:
        std::uint8_t* p_; ///< Pointer to the referenced storage byte (non-owning).
    };// boolRef

    // ---------- Free comparison & stream operators ----------

    /**
     * @brief Equality between two @c BoolRef proxies.
     * @param a Left-hand proxy.
     * @param b Right-hand proxy.
     * @return @c true iff both read the same boolean value.
     */
    bool operator==(const BoolRef& a, const BoolRef& b) noexcept;

    /**
     * @brief Inequality between two @c BoolRef proxies.
     * @param a Left-hand proxy.
     * @param b Right-hand proxy.
     * @return @c true iff the read boolean values differ.
     */
    bool operator!=(const BoolRef& a, const BoolRef& b) noexcept;

    /**
     * @brief Equality between a @c BoolRef and a @c bool (proxy on the left).
     * @param a Left-hand @c BoolRef proxy.
     * @param b Right-hand @c bool value.
     * @return @c true iff @p a reads the same value as @p b.
     */
    bool operator==(const BoolRef& a, bool b) noexcept;

    /**
     * @brief Inequality between a @c BoolRef and a @c bool (proxy on the left).
     * @param a Left-hand @c BoolRef proxy.
     * @param b Right-hand @c bool value.
     * @return @c true iff @p a and @p b differ.
     */
    bool operator!=(const BoolRef& a, bool b) noexcept;

    /**
     * @brief Equality between a @c bool and a @c BoolRef (proxy on the right).
     * @param a Left-hand @c bool value.
     * @param b Right-hand @c BoolRef proxy.
     * @return @c true iff @p a equals the value read from @p b.
     * @note Enables expressions like @c std::_Bit_reference == BoolRef (LHS converts to @c bool).
     */
    bool operator==(bool a, const BoolRef& b) noexcept;

    /**
     * @brief Inequality between a @c bool and a @c BoolRef (proxy on the right).
     * @param a Left-hand @c bool value.
     * @param b Right-hand @c BoolRef proxy.
     * @return @c true iff @p a and @p b differ.
     */
    bool operator!=(bool a, const BoolRef& b) noexcept;

    /**
     * @brief Stream output of the referenced boolean value.
     * @param os Output stream.
     * @param br Proxy to print.
     * @return @p os
     */
    std::ostream& operator<<(std::ostream& os, BoolRef br);



} // bml

#endif //BML_BOOLREF_HPP