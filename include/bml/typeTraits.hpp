#ifndef BML_TYPETRAITS_HPP
#define BML_TYPETRAITS_HPP

#include <cstdint>
#include <type_traits>
template <class U>
struct storage_of
{
    using type = U;
};

template <>
struct storage_of<bool>
{
    using type = std::uint8_t;
};

template <class U>
using storage_of_t = typename storage_of<U>::type;

// ---- helper traits to refine which Ts allow maths ----

// Arithmetic allowed for all arithmetic types EXCEPT plain char and bool.
// (signed char / unsigned char remain allowed; floats allowed; bool/char blocked)
template <typename X>
struct bml_is_math_arithmetic
    : std::bool_constant<
        std::is_arithmetic<X>::value &&
        !std::is_same<typename std::remove_cv<X>::type, char>::value &&
        !std::is_same<typename std::remove_cv<X>::type, bool>::value>
{
};

// Modulus allowed for integral types EXCEPT plain char and bool.
// (signed char / unsigned char remain allowed)
template <typename X>
struct bml_is_math_integral
    : std::bool_constant<
        std::is_integral<X>::value &&
        !std::is_same<typename std::remove_cv<X>::type, char>::value &&
        !std::is_same<typename std::remove_cv<X>::type, bool>::value>
{
};

template <typename X>
struct bml_is_bool : std::is_same<typename std::remove_cv<X>::type, bool>
{
};
#define BML_TYPETRAITS_HPP

#endif //BML_TYPETRAITS_HPP

