//
// Created by johan on 2025-09-26.
//

#include "bml/boolRef.hpp"

namespace bml
{

    BoolRef::BoolRef(std::uint8_t* p) noexcept : p_(p) {}

    BoolRef::operator bool() const noexcept { return *p_ != 0; }

    BoolRef& BoolRef::operator=(bool v) noexcept {
        *p_ = static_cast<std::uint8_t>(v ? 1 : 0);
        return *this;
    }

    BoolRef& BoolRef::operator=(const BoolRef& o) noexcept {
        if (this == &o) return *this;              // satisfy clang-tidy
        return (*this = static_cast<bool>(o));     // delegate to operator=(bool)
    }

    BoolRef& BoolRef::operator&=(bool v) noexcept {
        return *this = (static_cast<bool>(*this) && v);
    }
    BoolRef& BoolRef::operator|=(bool v) noexcept {
        return *this = (static_cast<bool>(*this) || v);
    }
    BoolRef& BoolRef::operator^=(bool v) noexcept {
        return *this = (static_cast<bool>(*this) ^ v);
    }



    bool operator==(const BoolRef& a, const BoolRef& b) noexcept
    {
        return  static_cast<bool>(a) == static_cast<bool>(b);
    }

    bool operator!=(const BoolRef& a, const BoolRef& b) noexcept
    {
        return static_cast<bool>(a) != static_cast<bool>(b);
    }

    bool operator==(const BoolRef& a, bool b) noexcept
    {
        return  static_cast<bool>(a) == b;
    }

    bool operator!=(const BoolRef& a, bool b) noexcept
    {
        return  static_cast<bool>(a) != b;
    }

    bool operator==(bool a, const BoolRef& b) noexcept
    {
        return a == static_cast<bool>(b);
    }

    bool operator!=(const bool a, const BoolRef& b) noexcept
    {
        return a != static_cast<bool>(b);
    }

    std::ostream& operator<<(std::ostream& os, BoolRef br)
    {
        return os << static_cast<bool>(br);
    }
} // bml