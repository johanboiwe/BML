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

        /// Compare to a bool by value.
        friend bool operator==(const BoolRef& a, bool b) noexcept;
        /// Negation of equality.
        friend bool operator!=(const BoolRef& a, bool b) noexcept;

    private:
        std::uint8_t* p_; ///< Pointer to the referenced storage byte (non-owning).
    };// boolRef

     // —— free operators so proxy–proxy and bool–proxy also work ——
     bool operator==(BoolRef a, BoolRef b) noexcept;
     bool operator!=(BoolRef a, BoolRef b) noexcept;
     bool operator==(bool a,  BoolRef b) noexcept;
     bool operator!=(bool a,  BoolRef b) noexcept;

     std::ostream& operator<<(std::ostream& os, const BoolRef& br);

} // bml

#endif //BML_BOOLREF_HPP