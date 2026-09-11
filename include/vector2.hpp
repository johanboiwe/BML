//
// Created by johan on 2026-09-11.
//

#ifndef BML_VECTOR2_HPP
#define BML_VECTOR2_HPP
#include <cmath>
#include <type_traits>
#include <half.hpp>

namespace bml {

template<typename T>
struct Vector2 {

    static_assert(
        std::is_floating_point_v<T>,
        "Vector2 requires a floating-point type"
    );

    T x{};
    T y{};


    constexpr Vector2() noexcept = default;

    constexpr Vector2(T x, T y) noexcept
        : x{x}, y{y}
    {}


    // ---------- Arithmetic ----------

    constexpr Vector2 operator+(const Vector2& other) const noexcept {
        return {x + other.x, y + other.y};
    }

    constexpr Vector2 operator-(const Vector2& other) const noexcept {
        return {x - other.x, y - other.y};
    }

    constexpr Vector2 operator-() const noexcept {
        return {-x, -y};
    }

    constexpr Vector2 operator*(T scalar) const noexcept {
        return {x * scalar, y * scalar};
    }

    constexpr Vector2 operator/(T scalar) const noexcept {
        return {x / scalar, y / scalar};
    }


    // ---------- Compound assignment ----------

    constexpr Vector2& operator+=(const Vector2& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vector2& operator-=(const Vector2& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vector2& operator*=(T scalar) noexcept {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vector2& operator/=(T scalar) noexcept {
        x /= scalar;
        y /= scalar;
        return *this;
    }


    // ---------- Vector operations ----------

    constexpr T dot(const Vector2& other) const noexcept {
        return x * other.x + y * other.y;
    }

    constexpr T cross(const Vector2& other) const noexcept {
        return x * other.y - y * other.x;
    }

    constexpr T lengthSquared() const noexcept {
        return x * x + y * y;
    }

    T length() const noexcept {
        using std::sqrt;
        return sqrt(lengthSquared());
    }


    // ---------- Comparison ----------

    constexpr bool operator==(const Vector2& other) const noexcept {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Vector2& other) const noexcept {
        return !(*this == other);
    }
};


// ---------- Scalar multiplication ----------

template<typename T>
constexpr Vector2<T> operator*(
    T scalar,
    const Vector2<T>& vector
) noexcept {
    return vector * scalar;
}


// ---------- Type aliases ----------

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

// Half precision is provided by half-therock.
using Vector2h = Vector2<half_float::half>;

}// namespace bml
#endif //BML_VECTOR2_HPP