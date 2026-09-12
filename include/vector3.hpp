#ifndef BML_VECTOR3_HPP
#define BML_VECTOR3_HPP

#include <cmath>
#include <type_traits>
#include <half.hpp>

namespace bml {

/**
 * @brief A three-dimensional vector with floating-point components.
 *
 * Vector3 represents a point, direction, or general three-dimensional
 * quantity using three floating-point values.
 *
 * The component type is supplied as the template parameter. BML provides
 * convenient aliases for the commonly used floating-point precisions:
 *
 * @code
 * bml::Vector3h halfVector{1.0f, 2.0f, 3.0f};
 * bml::Vector3f floatVector{1.0f, 2.0f, 3.0f};
 * bml::Vector3d doubleVector{1.0, 2.0, 3.0};
 * @endcode
 *
 * @tparam T The floating-point type used for all components.
 *
 * @note T must be a type recognised by std::is_floating_point_v.
 *
 * @par Arithmetic
 *
 * Vector3 supports the usual vector arithmetic operations:
 *
 * @code
 * Vector3f a{2.0f, 3.0f, 4.0f};
 * Vector3f b{4.0f, 5.0f, 6.0f};
 *
 * auto sum     = a + b;        // {6, 8, 10}
 * auto diff    = a - b;        // {-2, -2, -2}
 * auto neg     = -a;           // {-2, -3, -4}
 *
 * auto scaled  = a * 2.0f;     // {4, 6, 8}
 * auto divided = a / 2.0f;     // {1, 1.5, 2}
 * @endcode
 *
 * Scalar multiplication is commutative:
 *
 * @code
 * auto a = Vector3f{2.0f, 3.0f, 4.0f};
 *
 * auto b = a * 2.0f;
 * auto c = 2.0f * a;
 * @endcode
 *
 * Both expressions produce the same vector.
 *
 * @par Dot product
 *
 * The dot product is calculated using:
 *
 * @code
 * x1 * x2 + y1 * y2 + z1 * z2
 * @endcode
 *
 * and produces a scalar value:
 *
 * @code
 * Vector3f a{1.0f, 2.0f, 3.0f};
 * Vector3f b{4.0f, 5.0f, 6.0f};
 *
 * float result = a.dot(b); // 32
 * @endcode
 *
 * The dot product is useful for determining the relationship between
 * directions, including whether vectors point generally in the same or
 * opposite directions and whether they are perpendicular.
 *
 * @par Cross product
 *
 * In three dimensions, the cross product produces another Vector3 that
 * is perpendicular to both input vectors.
 *
 * The cross product is calculated using:
 *
 * @code
 * {
 *     y1 * z2 - z1 * y2,
 *     z1 * x2 - x1 * z2,
 *     x1 * y2 - y1 * x2
 * }
 * @endcode
 *
 * For example:
 *
 * @code
 * Vector3f a{1.0f, 0.0f, 0.0f};
 * Vector3f b{0.0f, 1.0f, 0.0f};
 *
 * auto result = a.cross(b); // {0, 0, 1}
 * @endcode
 *
 * The direction of the resulting vector follows the right-hand rule.
 *
 * @par Length
 *
 * lengthSquared() returns the squared magnitude without calculating a
 * square root. It is preferable when only relative lengths need to be
 * compared:
 *
 * @code
 * Vector3f a{2.0f, 3.0f, 6.0f};
 *
 * float squared = a.lengthSquared(); // 49
 * float length   = a.length();       // 7
 * @endcode
 *
 * @par Equality
 *
 * Equality compares all three components using their native == operators.
 * No epsilon or approximate comparison is performed.
 *
 * This means that:
 *
 * @code
 * a == b
 * @endcode
 *
 * is true only when both x, y, and z components compare equal.
 *
 * @note Floating-point equality should therefore be used with the usual
 * care associated with exact floating-point comparisons.
 */
template<typename T>
struct Vector3 {

    static_assert(
    std::is_floating_point_v<T> ||
    std::is_same_v<T, half_float::half>,
    "Vector3 requires float, double, or half"
);

    /**
     * @brief X component of the vector.
     */
    T x{};

    /**
     * @brief Y component of the vector.
     */
    T y{};

    /**
     * @brief Z component of the vector.
     */
    T z{};


    /**
     * @brief Constructs a zero vector.
     *
     * The components are value-initialised to zero.
     *
     * @code
     * Vector3f v;
     * // v == {0.0f, 0.0f, 0.0f}
     * @endcode
     */
    constexpr Vector3() noexcept = default;


    /**
     * @brief Constructs a vector from three components.
     *
     * @param x The x component.
     * @param y The y component.
     * @param z The z component.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     * @endcode
     */
    constexpr Vector3(T x, T y, T z) noexcept
        : x{x}, y{y}, z{z}
    {}


    // ---------- Arithmetic ----------

    /**
     * @brief Adds two vectors.
     *
     * @param other Vector to add.
     * @return The component-wise sum.
     *
     * @code
     * Vector3f a{1.0f, 2.0f, 3.0f};
     * Vector3f b{4.0f, 5.0f, 6.0f};
     *
     * auto result = a + b; // {5, 7, 9}
     * @endcode
     */
    constexpr Vector3 operator+(const Vector3& other) const noexcept {
        return {x + other.x, y + other.y, z + other.z};
    }


    /**
     * @brief Subtracts one vector from another.
     *
     * @param other Vector to subtract.
     * @return The component-wise difference.
     *
     * @code
     * Vector3f a{5.0f, 7.0f, 9.0f};
     * Vector3f b{2.0f, 3.0f, 4.0f};
     *
     * auto result = a - b; // {3, 4, 5}
     * @endcode
     */
    constexpr Vector3 operator-(const Vector3& other) const noexcept {
        return {x - other.x, y - other.y, z - other.z};
    }


    /**
     * @brief Negates all components.
     *
     * @return A vector pointing in the opposite direction.
     *
     * @code
     * Vector3f v{2.0f, -3.0f, 4.0f};
     * auto result = -v; // {-2, 3, -4}
     * @endcode
     */
    constexpr Vector3 operator-() const noexcept {
        return {-x, -y, -z};
    }


    /**
     * @brief Multiplies all components by a scalar.
     *
     * @param scalar Value by which to multiply all components.
     * @return The scaled vector.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     *
     * auto result = v * 2.0f; // {4, 6, 8}
     * @endcode
     */
    constexpr Vector3 operator*(T scalar) const noexcept {
        return {x * scalar, y * scalar, z * scalar};
    }


    /**
     * @brief Divides all components by a scalar.
     *
     * @param scalar Value by which to divide all components.
     * @return The scaled vector.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     *
     * @code
     * Vector3f v{4.0f, 6.0f, 8.0f};
     *
     * auto result = v / 2.0f; // {2, 3, 4}
     * @endcode
     */
    constexpr Vector3 operator/(T scalar) const noexcept {
        return {x / scalar, y / scalar, z / scalar};
    }


    // ---------- Compound assignment ----------

    /**
     * @brief Adds another vector to this vector.
     *
     * @param other Vector to add.
     * @return Reference to this vector.
     */
    constexpr Vector3& operator+=(const Vector3& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }


    /**
     * @brief Subtracts another vector from this vector.
     *
     * @param other Vector to subtract.
     * @return Reference to this vector.
     */
    constexpr Vector3& operator-=(const Vector3& other) noexcept {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }


    /**
     * @brief Multiplies all components by a scalar in place.
     *
     * @param scalar Value by which to multiply all components.
     * @return Reference to this vector.
     */
    constexpr Vector3& operator*=(T scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }


    /**
     * @brief Divides all components by a scalar in place.
     *
     * @param scalar Value by which to divide all components.
     * @return Reference to this vector.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     */
    constexpr Vector3& operator/=(T scalar) noexcept {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }


    // ---------- Vector operations ----------

    /**
     * @brief Calculates the dot product with another vector.
     *
     * The dot product is:
     *
     * @code
     * x * other.x + y * other.y + z * other.z
     * @endcode
     *
     * @param other Vector to calculate the dot product with.
     * @return The scalar dot product.
     *
     * @code
     * Vector3f a{1.0f, 2.0f, 3.0f};
     * Vector3f b{4.0f, 5.0f, 6.0f};
     *
     * float result = a.dot(b); // 32
     * @endcode
     */
    [[nodiscard]]
    constexpr T dot(const Vector3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }


    /**
     * @brief Calculates the three-dimensional cross product.
     *
     * The cross product produces a vector perpendicular to both input
     * vectors.
     *
     * The result is:
     *
     * @code
     * {
     *     y * other.z - z * other.y,
     *     z * other.x - x * other.z,
     *     x * other.y - y * other.x
     * }
     * @endcode
     *
     * @param other Vector to calculate the cross product with.
     * @return The vector cross product.
     *
     * @code
     * Vector3f a{1.0f, 0.0f, 0.0f};
     * Vector3f b{0.0f, 1.0f, 0.0f};
     *
     * auto result = a.cross(b); // {0, 0, 1}
     * @endcode
     *
     * @note The direction of the resulting vector follows the right-hand
     * rule.
     */
    [[nodiscard]]
    constexpr Vector3 cross(const Vector3& other) const noexcept {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }


    /**
     * @brief Calculates the squared length of the vector.
     *
     * This is equivalent to:
     *
     * @code
     * x * x + y * y + z * z
     * @endcode
     *
     * Unlike length(), this function does not perform a square-root
     * operation.
     *
     * This makes it useful when comparing distances or magnitudes where
     * the actual length is not required.
     *
     * @return The squared length of the vector.
     *
     * @code
     * Vector3f a{2.0f, 3.0f, 6.0f};
     *
     * float result = a.lengthSquared(); // 49
     * @endcode
     */
    [[nodiscard]]
    constexpr T lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }


    /**
     * @brief Calculates the length (magnitude) of the vector.
     *
     * The length is calculated as:
     *
     * @code
     * sqrt(x * x + y * y + z * z)
     * @endcode
     *
     * @return The vector's magnitude.
     *
     * @code
     * Vector3f a{2.0f, 3.0f, 6.0f};
     *
     * float result = a.length(); // 7
     * @endcode
     *
     * @note This function performs a square-root operation. If only
     * relative lengths need to be compared, consider lengthSquared()
     * instead.
     */
    [[nodiscard]]
    T length() const noexcept {
        using std::sqrt;
        return sqrt(lengthSquared());
    }


    // ---------- Comparison ----------

    /**
     * @brief Compares two vectors for exact equality.
     *
     * All three components must compare equal.
     *
     * No tolerance or epsilon is used.
     *
     * @param other Vector to compare with.
     * @return true if all components are equal; otherwise false.
     */
    constexpr bool operator==(const Vector3& other) const noexcept {
        return x == other.x &&
               y == other.y &&
               z == other.z;
    }


    /**
     * @brief Compares two vectors for inequality.
     *
     * This is equivalent to:
     *
     * @code
     * !(*this == other)
     * @endcode
     *
     * @param other Vector to compare with.
     * @return true if the vectors are not equal; otherwise false.
     */
    constexpr bool operator!=(const Vector3& other) const noexcept {
        return !(*this == other);
    }
};


// ---------- Scalar multiplication ----------

/**
 * @brief Multiplies a vector by a scalar.
 *
 * Provides scalar multiplication with the scalar on the left-hand side.
 *
 * @param scalar Value by which to multiply all components.
 * @param vector Vector to scale.
 * @return The scaled vector.
 *
 * @code
 * Vector3f v{2.0f, 3.0f, 4.0f};
 *
 * auto result = 2.0f * v; // {4, 6, 8}
 * @endcode
 *
 * This is equivalent to:
 *
 * @code
 * v * 2.0f
 * @endcode
 */
template<typename T>
constexpr Vector3<T> operator*(
    T scalar,
    const Vector3<T>& vector
) noexcept {
    return vector * scalar;
}


// ---------- Type aliases ----------

/**
 * @brief Three-dimensional vector using half-precision floating-point values.
 *
 * The half-precision implementation is provided by the external
 * half-therock library.
 *
 * @see Vector3
 */
using Vector3h = Vector3<half_float::half>;


/**
 * @brief Three-dimensional vector using single-precision floating-point values.
 *
 * This is an alias for Vector3<float>.
 *
 * @see Vector3
 */
using Vector3f = Vector3<float>;


/**
 * @brief Three-dimensional vector using double-precision floating-point values.
 *
 * This is an alias for Vector3<double>.
 *
 * @see Vector3
 */
using Vector3d = Vector3<double>;

} // namespace bml

#endif // BML_VECTOR3_HPP