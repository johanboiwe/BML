#ifndef BML_TRAVERSAL_HPP
#define BML_TRAVERSAL_HPP

#include <cstdint>

namespace bml {

    /**
     * @brief Describes how to traverse a matrix.
     *
     * Row:
     *   row-major order (left→right, then next row)
     *
     * Column:
     *   column-major order (top→bottom, then next column)
     *
     * Diagonal:
     *   along main diagonals (top-left → bottom-right)
     *
     * AntiDiagonal:
     *   along anti-diagonals (top-right → bottom-left)
     *
     * The underlying type is std::uint8_t to keep this enum 1 byte wide.
     * This matters if many of these are stored in memory.
     */
    enum class TraversalType : std::uint8_t {
        Row = 0,
        Column = 1,
        Diagonal = 2,
        AntiDiagonal = 3
    };

} // namespace bml

#endif // BML_TRAVERSAL_HPP
