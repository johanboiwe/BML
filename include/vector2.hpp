#ifndef BML_VECTOR2_HPP
#define BML_VECTOR2_HPP

#include <cmath>
#include <ostream>
#include <type_traits>

#include <half.hpp>

namespace bml {

/**
 * @brief A two-dimensional vector with floating-point components.
 *
 * Vector2 represents a point, direction, or general two-dimensional
 * quantity using two floating-point values.
 *
 * The component type is supplied as the template parameter. BML provides
 * convenient aliases for the commonly used floating-point precisions:
 *
 * @code
 * bml::Vector2h halfVector{1.0f, 2.0f};
 * bml::Vector2f floatVector{1.0f, 2.0f};
 * bml::Vector2d doubleVector{1.0, 2.0};
 * @endcode
 *
 * @tparam T The floating-point type used for both components.
 *
 * @note T must be float, double, or half_float::half.
 *
 * @par Special member functions
 *
 * Vector2 explicitly defaults all five special member functions.
 * Copy and move operations perform member-wise operations on the vector
 * components. Vector2 does not own or manage any external resources.
 *
 * @code
 * Vector2f a{1.0f, 2.0f};
 * Vector2f b = a;            // copy construction
 * Vector2f c = std::move(a); // move construction
 *
 * b = c;                     // copy assignment
 * b = std::move(c);          // move assignment
 * @endcode
 *
 * The explicitly defaulted special member functions provide the normal
 * value-type semantics expected of a vector.
 *
 * @par Arithmetic
 *
 * Vector2 supports component-wise vector arithmetic and scalar arithmetic:
 *
 * @code
 * Vector2f a{2.0f, 3.0f};
 * Vector2f b{4.0f, 5.0f};
 *
 * auto sum        = a + b;       // {6, 8}
 * auto diff       = a - b;       // {-2, -2}
 * auto neg        = -a;          // {-2, -3}
 * auto multiplied = a * b;       // {8, 15}
 * auto divided    = b / a;       // {2, 1.66667}
 *
 * auto added      = a + 2.0f;    // {4, 5}
 * auto subtracted = a - 2.0f;    // {0, 1}
 * auto scaled     = a * 2.0f;    // {4, 6}
 * auto scalarDiv  = a / 2.0f;    // {1, 1.5}
 * @endcode
 *
 * Vector multiplication and division are component-wise. They do not
 * calculate a dot product.
 *
 * @par Scalar multiplication
 *
 * Scalar multiplication is commutative:
 *
 * @code
 * Vector2f a{2.0f, 3.0f};
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
 * x1 * x2 + y1 * y2
 * @endcode
 *
 * and produces a scalar value:
 *
 * @code
 * Vector2f a{1.0f, 2.0f};
 * Vector2f b{3.0f, 4.0f};
 *
 * float result = a.dot(b); // 11
 * @endcode
 *
 * The dot product is deliberately provided as dot() rather than
 * operator* because operator* performs component-wise multiplication.
 *
 * @par 2D cross product
 *
 * In two dimensions, the cross product produces a scalar rather than
 * another vector:
 *
 * @code
 * Vector2f a{1.0f, 0.0f};
 * Vector2f b{0.0f, 1.0f};
 *
 * float result = a.cross(b); // 1
 * @endcode
 *
 * The sign of the result indicates the orientation from one vector to
 * the other.
 *
 * @par Length
 *
 * lengthSquared() returns the squared magnitude without calculating a
 * square root. It is preferable when only relative lengths need to be
 * compared:
 *
 * @code
 * Vector2f a{3.0f, 4.0f};
 *
 * float squared = a.lengthSquared(); // 25
 * float length   = a.length();       // 5
 * @endcode
 *
 * @par Equality
 *
 * Equality compares both components using their native == operators.
 * No epsilon or approximate comparison is performed.
 *
 * This means that:
 *
 * @code
 * a == b
 * @endcode
 *
 * is true only when both x components compare equal and both y components
 * compare equal.
 *
 * @note Floating-point equality should therefore be used with the usual
 * care associated with exact floating-point comparisons.
 */
template<typename T>
struct Vector2 {

    static_assert(
        std::is_floating_point_v<T> ||
        std::is_same_v<T, half_float::half>,
        "Vector2 requires float, double, or half"
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
     * @brief Constructs a zero vector.
     *
     * The components are value-initialised to zero.
     *
     * @code
     * Vector2f v;
     * // v == {0.0f, 0.0f}
     * @endcode
     */
    constexpr Vector2() noexcept = default;


    /**
     * @brief Constructs a vector from two components.
     *
     * @param x The x component.
     * @param y The y component.
     *
     * @code
     * Vector2f v{2.0f, 3.0f};
     * @endcode
     */
    constexpr Vector2(T x, T y) noexcept
        : x{x}, y{y}
    {}
    // ---------- Special member functions ----------

    /**
     * @brief Copy constructor.
     *
     * Performs a member-wise copy of the vector components.
     */
    Vector2(const Vector2&)            = default;

    /**
     * @brief Move constructor.
     *
     * Performs a member-wise move of the vector components.
     */
    Vector2(Vector2&&)                 = default;

    /**
     * @brief Copy assignment operator.
     *
     * Performs a member-wise copy of the vector components.
     */
    Vector2& operator=(const Vector2&) = default;

    /**
     * @brief Move assignment operator.
     *
     * Performs a member-wise move of the vector components.
     */
    Vector2& operator=(Vector2&&)      = default;

    /**
     * @brief Destructor.
     */
    ~Vector2()                         = default;
    // ---------- Arithmetic ----------

    /**
     * @brief Adds two vectors component-wise.
     *
     * @param other Vector to add.
     * @return The component-wise sum.
     *
     * @code
     * Vector2f a{1.0f, 2.0f};
     * Vector2f b{3.0f, 4.0f};
     *
     * auto result = a + b; // {4, 6}
     * @endcode
     */
    constexpr Vector2 operator+(const Vector2& other) const noexcept {
        return {x + other.x, y + other.y};
    }


    /**
     * @brief Adds a scalar to every component.
     *
     * @param scalar Value added to x and y.
     * @return The vector with the scalar added component-wise.
     *
     * @code
     * Vector2f v{2.0f, 3.0f};
     *
     * auto result = v + 2.0f; // {4, 5}
     * @endcode
     */
    constexpr Vector2 operator+(T scalar) const noexcept {
        return {x + scalar, y + scalar};
    }


    /**
     * @brief Subtracts two vectors component-wise.
     *
     * @param other Vector to subtract.
     * @return The component-wise difference.
     *
     * @code
     * Vector2f a{5.0f, 7.0f};
     * Vector2f b{2.0f, 3.0f};
     *
     * auto result = a - b; // {3, 4}
     * @endcode
     */
    constexpr Vector2 operator-(const Vector2& other) const noexcept {
        return {x - other.x, y - other.y};
    }


    /**
     * @brief Subtracts a scalar from every component.
     *
     * @param scalar Value subtracted from x and y.
     * @return The vector with the scalar subtracted component-wise.
     *
     * @code
     * Vector2f v{2.0f, 3.0f};
     *
     * auto result = v - 2.0f; // {0, 1}
     * @endcode
     */
    constexpr Vector2 operator-(T scalar) const noexcept {
        return {x - scalar, y - scalar};
    }


    /**
     * @brief Negates both components.
     *
     * @return A vector pointing in the opposite direction.
     *
     * @code
     * Vector2f v{2.0f, -3.0f};
     * auto result = -v; // {-2, 3}
     * @endcode
     */
    constexpr Vector2 operator-() const noexcept {
        return {-x, -y};
    }


    /**
     * @brief Multiplies two vectors component-wise.
     *
     * This is not a dot product. Each component is multiplied
     * independently.
     *
     * @param other Vector whose components are multiplied with this vector.
     * @return The component-wise product.
     *
     * @code
     * Vector2f a{2.0f, 1.0f};
     * Vector2f b{2.0f, 2.0f};
     *
     * auto result = a * b; // {4, 2}
     * @endcode
     */
    constexpr Vector2 operator*(const Vector2& other) const noexcept {
        return {
            x * other.x,
            y * other.y
        };
    }


    /**
     * @brief Multiplies both components by a scalar.
     *
     * @param scalar Value by which to multiply both components.
     * @return The scaled vector.
     *
     * @code
     * Vector2f v{2.0f, 3.0f};
     *
     * auto result = v * 2.0f; // {4, 6}
     * @endcode
     */
    constexpr Vector2 operator*(T scalar) const noexcept {
        return {x * scalar, y * scalar};
    }


    /**
     * @brief Divides two vectors component-wise.
     *
     * @param other Vector whose components are used as divisors.
     * @return The component-wise quotient.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     *
     * @code
     * Vector2f a{4.0f, 6.0f};
     * Vector2f b{2.0f, 2.0f};
     *
     * auto result = a / b; // {2, 3}
     * @endcode
     */
    constexpr Vector2 operator/(const Vector2& other) const noexcept {
        return {
            x / other.x,
            y / other.y
        };
    }


    /**
     * @brief Divides both components by a scalar.
     *
     * @param scalar Value by which to divide both components.
     * @return The scaled vector.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     *
     * @code
     * Vector2f v{4.0f, 6.0f};
     *
     * auto result = v / 2.0f; // {2, 3}
     * @endcode
     */
    constexpr Vector2 operator/(T scalar) const noexcept {
        return {x / scalar, y / scalar};
    }


    // ---------- Compound assignment ----------

    /**
     * @brief Adds another vector to this vector component-wise.
     *
     * @param other Vector to add.
     * @return Reference to this vector.
     */
    constexpr Vector2& operator+=(const Vector2& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }


    /**
     * @brief Adds a scalar to every component in place.
     *
     * @param scalar Value added to x and y.
     * @return Reference to this vector.
     *
     * @code
     * Vector2f v{2.0f, 3.0f};
     *
     * v += 2.0f; // {4, 5}
     * @endcode
     */
    constexpr Vector2& operator+=(T scalar) noexcept {
        x += scalar;
        y += scalar;
        return *this;
    }


    /**
     * @brief Subtracts another vector from this vector component-wise.
     *
     * @param other Vector to subtract.
     * @return Reference to this vector.
     */
    constexpr Vector2& operator-=(const Vector2& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }


    /**
     * @brief Subtracts a scalar from every component in place.
     *
     * @param scalar Value subtracted from x and y.
     * @return Reference to this vector.
     *
     * @code
     * Vector2f v{2.0f, 3.0f};
     *
     * v -= 2.0f; // {0, 1}
     * @endcode
     */
    constexpr Vector2& operator-=(T scalar) noexcept {
        x -= scalar;
        y -= scalar;
        return *this;
    }


    /**
     * @brief Multiplies this vector by another vector component-wise.
     *
     * @param other Vector whose components are multiplied with this vector.
     * @return Reference to this vector.
     *
     * @code
     * Vector2f a{2.0f, 1.0f};
     * Vector2f b{2.0f, 2.0f};
     *
     * a *= b; // {4, 2}
     * @endcode
     */
    constexpr Vector2& operator*=(const Vector2& other) noexcept {
        x *= other.x;
        y *= other.y;
        return *this;
    }


    /**
     * @brief Multiplies both components by a scalar in place.
     *
     * @param scalar Value by which to multiply both components.
     * @return Reference to this vector.
     */
    constexpr Vector2& operator*=(T scalar) noexcept {
        x *= scalar;
        y *= scalar;
        return *this;
    }


    /**
     * @brief Divides this vector by another vector component-wise.
     *
     * @param other Vector whose components are used as divisors.
     * @return Reference to this vector.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     */
    constexpr Vector2& operator/=(const Vector2& other) noexcept {
        x /= other.x;
        y /= other.y;
        return *this;
    }


    /**
     * @brief Divides both components by a scalar in place.
     *
     * @param scalar Value by which to divide both components.
     * @return Reference to this vector.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     */
    constexpr Vector2& operator/=(T scalar) noexcept {
        x /= scalar;
        y /= scalar;
        return *this;
    }


    // ---------- Vector operations ----------

    /**
     * @brief Calculates the dot product with another vector.
     *
     * The dot product is:
     *
     * @code
     * x * other.x + y * other.y
     * @endcode
     *
     * @param other Vector to calculate the dot product with.
     * @return The scalar dot product.
     *
     * @code
     * Vector2f a{1.0f, 2.0f};
     * Vector2f b{3.0f, 4.0f};
     *
     * float result = a.dot(b); // 11
     * @endcode
     */
    [[nodiscard]]
    constexpr T dot(const Vector2& other) const noexcept {
        return x * other.x + y * other.y;
    }


    /**
     * @brief Calculates the two-dimensional cross product.
     *
     * In two dimensions the cross product is represented by a scalar:
     *
     * @code
     * x * other.y - y * other.x
     * @endcode
     *
     * The sign of the result indicates the orientation between the two
     * vectors.
     *
     * @param other Vector to calculate the cross product with.
     * @return The scalar cross product.
     *
     * @code
     * Vector2f a{1.0f, 0.0f};
     * Vector2f b{0.0f, 1.0f};
     *
     * float result = a.cross(b); // 1
     * @endcode
     */
    [[nodiscard]]
    constexpr T cross(const Vector2& other) const noexcept {
        return x * other.y - y * other.x;
    }


    /**
     * @brief Calculates the squared length of the vector.
     *
     * This is equivalent to:
     *
     * @code
     * x * x + y * y
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
     * Vector2f a{3.0f, 4.0f};
     *
     * float result = a.lengthSquared(); // 25
     * @endcode
     */
    [[nodiscard]]
    constexpr T lengthSquared() const noexcept {
        return x * x + y * y;
    }


    /**
     * @brief Calculates the length (magnitude) of the vector.
     *
     * The length is calculated as:
     *
     * @code
     * sqrt(x * x + y * y)
     * @endcode
     *
     * @return The vector's magnitude.
     *
     * @code
     * Vector2f a{3.0f, 4.0f};
     *
     * float result = a.length(); // 5
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
     * @brief Checks whether both components are zero.
     *
     * @return true if both x and y are zero; otherwise false.
     */
    [[nodiscard]]
    constexpr bool isZero() const noexcept {
        return x == T{} && y == T{};
    }


    /**
     * @brief Compares two vectors for exact equality.
     *
     * Both x and y components must compare equal.
     *
     * No tolerance or epsilon is used.
     *
     * @param other Vector to compare with.
     * @return true if both components are equal; otherwise false.
     */
    constexpr bool operator==(const Vector2& other) const noexcept {
        return x == other.x && y == other.y;
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
    constexpr bool operator!=(const Vector2& other) const noexcept {
        return !(*this == other);
    }
};


// ---------- Stream output ----------

/**
 * @brief Writes a Vector2 to an output stream.
 *
 * The vector is formatted as:
 *
 * @code
 * {x, y}
 * @endcode
 *
 * @param os Output stream.
 * @param vector Vector to write.
 * @return Reference to the output stream.
 *
 * @code
 * Vector2f v{1.0f, 2.0f};
 * std::cout << v; // {1, 2}
 * @endcode
 */
template<typename T>
std::ostream& operator<<(
    std::ostream& os,
    const Vector2<T>& vector
) {
    return os << '{'
              << vector.x << ", "
              << vector.y
              << '}';
}


// ---------- Scalar multiplication ----------

/**
 * @brief Multiplies a vector by a scalar.
 *
 * Provides scalar multiplication with the scalar on the left-hand side.
 *
 * @param scalar Value by which to multiply both components.
 * @param vector Vector to scale.
 * @return The scaled vector.
 *
 * @code
 * Vector2f v{2.0f, 3.0f};
 *
 * auto result = 2.0f * v; // {4, 6}
 * @endcode
 *
 * This is equivalent to:
 *
 * @code
 * v * 2.0f
 * @endcode
 */
template<typename T>
constexpr Vector2<T> operator*(
    T scalar,
    const Vector2<T>& vector
) noexcept {
    return vector * scalar;
}


// ---------- Type aliases ----------

/**
 * @brief Two-dimensional vector using half-precision floating-point values.
 *
 * The half-precision implementation is provided by the half library.
 *
 * @see Vector2
 */
using Vector2h = Vector2<half_float::half>;


/**
 * @brief Two-dimensional vector using single-precision floating-point values.
 *
 * This is an alias for Vector2<float>.
 *
 * @see Vector2
 */
using Vector2f = Vector2<float>;


/**
 * @brief Two-dimensional vector using double-precision floating-point values.
 *
 * This is an alias for Vector2<double>.
 *
 * @see Vector2
 */
using Vector2d = Vector2<double>;

} // namespace bml

#endif // BML_VECTOR2_HPP
