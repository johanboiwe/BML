#ifndef BML_TYPETRAITS_HPP
#define BML_TYPETRAITS_HPP

#include <cstdint>
#include <type_traits>

/**
 * @file bml/typeTraits.hpp
 * @brief Internal type traits used by BML for storage layout and operator enablement.
 *
 * This header provides:
 *   - `storage_of<T>`: maps a public/visible element type `T` to the type actually
 *     stored internally in the matrix.
 *     - Example: `bool` is stored as `std::uint8_t`.
 *
 *   - Traits that control which element types are allowed to use certain math
 *     operators, e.g. addition, modulus, etc.
 *
 * These traits are mainly intended for use inside BML, but are documented
 * because advanced users may rely on them when extending the library.

 */
/// @cond INTERNAL
/**
 * @brief Trait that defines the underlying storage type used by BML for a given element type `U`.
 *
 * By default, `storage_of<U>::type` is just `U`. Specialisations may override
 * this to alter how certain types are stored in memory.
 *
 * @tparam U Logical (public) element type.
 *
 * @note
 * Example:
 * @code
 * using Raw = storage_of_t<bool>;      // Raw == std::uint8_t
 * using Raw2 = storage_of_t<float>;    // Raw2 == float
 * @endcode
 */
template <class U>
struct storage_of
{
    /// The physical storage type used internally for elements of type U.
    using type = U;
};

/**
 * @brief Specialisation of storage_of for bool.
 *
 * BML does not store bool as `bool`, because `bool` can be bit-packed or have
 * surprising behaviour. We explicitly store each bool in one byte.
 *
 * So `Matrix<bool>` internally keeps `std::uint8_t` per cell.
 */
template <>
struct storage_of<bool>
{
    using type = std::uint8_t;
};

/**
 * @brief Convenience alias for `storage_of<U>::type`.
 *
 * @tparam U Logical (public) element type.
 */
template <class U>
using storage_of_t = typename storage_of<U>::type;


// ---- helper traits to refine which Ts allow maths ----

/**
 * @brief Trait that reports whether a type `X` is allowed to use "general arithmetic"
 *        operators in BML (e.g. +, -, *, /, etc.).
 *
 * Rules:
 * - true for all arithmetic types except:
 *   - plain `char`
 *   - `bool`
 *
 * This means:
 *   - `int`, `unsigned`, `float`, `double`, `signed char`, `unsigned char` => allowed
 *   - `char` (the plain character type) => not allowed
 *   - `bool` => not allowed
 *
 * @tparam X candidate element type
 *
 * @note
 * Used with `std::enable_if_t<bml_is_math_arithmetic<T>::value, ...>` to make
 * certain operator overloads only appear for numeric-ish types and not for
 * logical or textual types.
 */
template <typename X>
struct bml_is_math_arithmetic
    : std::bool_constant<
        std::is_arithmetic<X>::value &&
        !std::is_same<typename std::remove_cv<X>::type, char>::value &&
        !std::is_same<typename std::remove_cv<X>::type, bool>::value>
{
};


/**
 * @brief Trait that reports whether a type `X` is allowed to use "integral-only"
 *        math in BML (e.g. modulus `%`).
 *
 * Rules:
 * - true for integral types except:
 *   - plain `char`
 *   - `bool`
 *
 * This means:
 *   - `int`, `unsigned`, `std::uint32_t`, `signed char`, `unsigned char` => allowed
 *   - `char` (plain char) => not allowed
 *   - `bool` => not allowed
 *   - floating point types => not allowed
 *
 * @tparam X candidate element type
 *
 * @note
 * This is typically used to guard operators like `%` which make sense only
 * for integers.
 */
template <typename X>
struct bml_is_math_integral
    : std::bool_constant<
        std::is_integral<X>::value &&
        !std::is_same<typename std::remove_cv<X>::type, char>::value &&
        !std::is_same<typename std::remove_cv<X>::type, bool>::value>
{
};


/**
 * @brief Trait that reports whether a type `X` is (cv-qualified) bool.
 *
 * @tparam X candidate type
 *
 * @note
 * This is mainly used to specialise behaviour for `Matrix<bool>`, which has
 * custom storage and semantics.
 *
 * Example:
 * @code
 * bml_is_bool<bool>::value           // true
 * bml_is_bool<const bool>::value     // true
 * bml_is_bool<int>::value            // false
 * @endcode
 */
template <typename X>
struct bml_is_bool : std::is_same<typename std::remove_cv<X>::type, bool>
{
};
/// @endcond
#endif // BML_TYPETRAITS_HPP
