#ifndef BML_VECTOR3_HPP
#define BML_VECTOR3_HPP

#include <cmath>
#include <ostream>
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
 * @note T must be float, double, or half_float::half.
 *
 * @par Special member functions
 *
 * Vector3 explicitly defaults all five special member functions.
 * Copy and move operations perform member-wise operations on the vector
 * components. Vector3 does not own or manage any external resources.
 *
 * @code
 * Vector3f a{1.0f, 2.0f, 3.0f};
 * Vector3f b = a;            // copy construction
 * Vector3f c = std::move(a); // move construction
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
 * Vector3 supports component-wise vector arithmetic and scalar arithmetic:
 *
 * @code
 * Vector3f a{2.0f, 3.0f, 4.0f};
 * Vector3f b{4.0f, 5.0f, 6.0f};
 *
 * auto sum        = a + b;       // {6, 8, 10}
 * auto diff       = a - b;       // {-2, -2, -2}
 * auto neg        = -a;          // {-2, -3, -4}
 * auto multiplied = a * b;       // {8, 15, 24}
 * auto divided    = b / a;       // {2, 1.66667, 1.5}
 *
 * auto added      = a + 2.0f;    // {4, 5, 6}
 * auto subtracted = a - 2.0f;    // {0, 1, 2}
 * auto scaled     = a * 2.0f;    // {4, 6, 8}
 * auto scalarDiv  = a / 2.0f;    // {1, 1.5, 2}
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
 * Vector3f a{2.0f, 3.0f, 4.0f};
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
 * The dot product is deliberately provided as dot() rather than
 * operator* because operator* performs component-wise multiplication.
 *
 * @par 3D cross product
 *
 * In three dimensions, the cross product produces another Vector3:
 *
 * @code
 * Vector3f a{1.0f, 0.0f, 0.0f};
 * Vector3f b{0.0f, 1.0f, 0.0f};
 *
 * Vector3f result = a.cross(b); // {0, 0, 1}
 * @endcode
 *
 * The resulting vector is perpendicular to both input vectors, with its
 * direction determined by the right-hand rule.
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
 * Equality compares all components using their native == operators.
 * No epsilon or approximate comparison is performed.
 *
 * This means that:
 *
 * @code
 * a == b
 * @endcode
 *
 * is true only when the x, y, and z components compare equal.
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
     */
    constexpr Vector3() noexcept = default;


    /**
     * @brief Constructs a vector from three components.
     *
     * @param x The x component.
     * @param y The y component.
     * @param z The z component.
     */
    constexpr Vector3(T x, T y, T z) noexcept
        : x{x}, y{y}, z{z}
    {}


        // ---------- Special member functions ----------

        /**
         * @brief Copy constructor.
         *
         * Performs a member-wise copy of the vector components.
         */
        Vector3(const Vector3&)            = default;

    /**
     * @brief Move constructor.
     *
     * Performs a member-wise move of the vector components.
     */
    Vector3(Vector3&&)                 = default;

    /**
     * @brief Copy assignment operator.
     *
     * Performs a member-wise copy of the vector components.
     */
    Vector3& operator=(const Vector3&) = default;

    /**
     * @brief Move assignment operator.
     *
     * Performs a member-wise move of the vector components.
     */
    Vector3& operator=(Vector3&&)      = default;

    /**
     * @brief Destructor.
     */
    ~Vector3()                         = default;


    // ---------- Arithmetic ----------

    /**
     * @brief Adds two vectors component-wise.
     *
     * @param other Vector to add.
     * @return The component-wise sum.
     */
    constexpr Vector3 operator+(const Vector3& other) const noexcept {
        return {x + other.x, y + other.y, z + other.z};
    }


    /**
     * @brief Adds a scalar to every component.
     *
     * @param scalar Value added to x, y, and z.
     * @return The vector with the scalar added to every component.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     *
     * auto result = v + 2.0f; // {4, 5, 6}
     * @endcode
     */
    constexpr Vector3 operator+(T scalar) const noexcept {
        return {x + scalar, y + scalar, z + scalar};
    }


    /**
     * @brief Subtracts two vectors component-wise.
     *
     * @param other Vector to subtract.
     * @return The component-wise difference.
     */
    constexpr Vector3 operator-(const Vector3& other) const noexcept {
        return {x - other.x, y - other.y, z - other.z};
    }


    /**
     * @brief Subtracts a scalar from every component.
     *
     * @param scalar Value subtracted from x, y, and z.
     * @return The vector with the scalar subtracted from every component.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     *
     * auto result = v - 2.0f; // {0, 1, 2}
     * @endcode
     */
    constexpr Vector3 operator-(T scalar) const noexcept {
        return {x - scalar, y - scalar, z - scalar};
    }


    /**
     * @brief Negates all components.
     *
     * @return The vector with every component negated.
     */
    constexpr Vector3 operator-() const noexcept {
        return {-x, -y, -z};
    }


    /**
     * @brief Multiplies two vectors component-wise.
     *
     * This is not a dot product.
     *
     * @param other Vector to multiply by.
     * @return The component-wise product.
     *
     * @code
     * Vector3f a{2.0f, 1.0f, 3.0f};
     * Vector3f b{2.0f, 2.0f, 4.0f};
     *
     * auto result = a * b; // {4, 2, 12}
     * @endcode
     */
    constexpr Vector3 operator*(const Vector3& other) const noexcept {
        return {
            x * other.x,
            y * other.y,
            z * other.z
        };
    }


    /**
     * @brief Multiplies all components by a scalar.
     *
     * @param scalar Value by which x, y, and z are multiplied.
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
     * @brief Divides two vectors component-wise.
     *
     * @param other Vector to divide by.
     * @return The component-wise quotient.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     *
     * @code
     * Vector3f a{2.0f, 3.0f, 4.0f};
     * Vector3f b{4.0f, 5.0f, 2.0f};
     *
     * auto result = a / b; // {0.5, 0.6, 2}
     * @endcode
     */
    constexpr Vector3 operator/(const Vector3& other) const noexcept {
        return {
            x / other.x,
            y / other.y,
            z / other.z
        };
    }


    /**
     * @brief Divides all components by a scalar.
     *
     * @param scalar Value by which x, y, and z are divided.
     * @return The scaled vector.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     *
     * auto result = v / 2.0f; // {1, 1.5, 2}
     * @endcode
     */
    constexpr Vector3 operator/(T scalar) const noexcept {
        return {x / scalar, y / scalar, z / scalar};
    }


    // ---------- Compound assignment ----------

    /**
     * @brief Adds another vector component-wise in place.
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
     * @brief Adds a scalar to every component in place.
     *
     * @param scalar Value added to x, y, and z.
     * @return Reference to this vector.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     *
     * v += 2.0f; // {4, 5, 6}
     * @endcode
     */
    constexpr Vector3& operator+=(T scalar) noexcept {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }


    /**
     * @brief Subtracts another vector component-wise in place.
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
     * @brief Subtracts a scalar from every component in place.
     *
     * @param scalar Value subtracted from x, y, and z.
     * @return Reference to this vector.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     *
     * v -= 2.0f; // {0, 1, 2}
     * @endcode
     */
    constexpr Vector3& operator-=(T scalar) noexcept {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }


    /**
     * @brief Multiplies this vector by another vector component-wise.
     *
     * @param other Vector to multiply by.
     * @return Reference to this vector.
     */
    constexpr Vector3& operator*=(const Vector3& other) noexcept {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }


    /**
     * @brief Multiplies all components by a scalar in place.
     *
     * @param scalar Value by which x, y, and z are multiplied.
     * @return Reference to this vector.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     *
     * v *= 2.0f; // {4, 6, 8}
     * @endcode
     */
    constexpr Vector3& operator*=(T scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }


    /**
     * @brief Divides this vector by another vector component-wise.
     *
     * @param other Vector to divide by.
     * @return Reference to this vector.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     */
    constexpr Vector3& operator/=(const Vector3& other) noexcept {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }


    /**
     * @brief Divides all components by a scalar in place.
     *
     * @param scalar Value by which x, y, and z are divided.
     * @return Reference to this vector.
     *
     * @warning Division by zero follows the behaviour of the underlying
     * floating-point type.
     *
     * @code
     * Vector3f v{2.0f, 3.0f, 4.0f};
     *
     * v /= 2.0f; // {1, 1.5, 2}
     * @endcode
     */
    constexpr Vector3& operator/=(T scalar) noexcept {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }


    // ---------- Vector operations ----------

    /**
     * @brief Checks whether all components are zero.
     *
     * @return true if x, y, and z are all zero; otherwise false.
     */
    [[nodiscard]]
    constexpr bool isZero() const noexcept {
        return x == T{} &&
               y == T{} &&
               z == T{};
    }


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
     */
    [[nodiscard]]
    constexpr T dot(const Vector3& other) const noexcept {
        return x * other.x +
               y * other.y +
               z * other.z;
    }


    /**
     * @brief Calculates the three-dimensional cross product.
     *
     * The cross product produces a vector perpendicular to both input
     * vectors.
     *
     * @param other Vector to calculate the cross product with.
     * @return The cross product.
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
     * @return The squared magnitude of the vector.
     */
    [[nodiscard]]
    constexpr T lengthSquared() const noexcept {
        return x * x +
               y * y +
               z * z;
    }


    /**
     * @brief Calculates the length (magnitude) of the vector.
     *
     * @return The magnitude of the vector.
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
     * @param other Vector to compare with.
     * @return true if all three components compare equal.
     */
    constexpr bool operator==(const Vector3& other) const noexcept {
        return x == other.x &&
               y == other.y &&
               z == other.z;
    }


    /**
     * @brief Compares two vectors for inequality.
     *
     * @param other Vector to compare with.
     * @return true if any component differs.
     */
    constexpr bool operator!=(const Vector3& other) const noexcept {
        return !(*this == other);
    }
};


// ---------- Stream output ----------

/**
 * @brief Writes a Vector3 to an output stream.
 *
 * The vector is formatted as:
 *
 * @code
 * {x, y, z}
 * @endcode
 */
template<typename T>
std::ostream& operator<<(
    std::ostream& os,
    const Vector3<T>& vector
) {
    return os << '{'
              << vector.x << ", "
              << vector.y << ", "
              << vector.z
              << '}';
}


// ---------- Scalar multiplication ----------

/**
 * @brief Multiplies a vector by a scalar.
 *
 * Provides scalar multiplication with the scalar on the left-hand side.
 *
 * @param scalar Scalar multiplier.
 * @param vector Vector to multiply.
 * @return The scaled vector.
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
 * The half-precision implementation is provided by the half library.
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
