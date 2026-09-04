#ifndef BML_ITERATOR_HPP
#define BML_ITERATOR_HPP

/**
 * @file iterator.hpp
 * @brief Iterators and iterator-related helpers for BML matrices.
 *
 * This header defines:
 *
 * - `element_ref_t<T>` and `const_element_t<T>`:
 *   helper aliases that deduce the correct reference/proxy type when
 *   accessing a `Matrix<T>` cell (normal `T&` for most T, `BoolRef` for `bool`,
 *   plain `bool` for const access to `Matrix<bool>`, etc.).
 *
 * - `ConstMatrixIterator<T>` / `MatrixIterator<T>`:
 *   forward iterators that walk a `Matrix<T>` in a given `TraversalType`
 *   (row-major, column-major, etc.). Dereferencing yields a tuple
 *   `(row, col, value_or_reference)`.
 *
 * - `BoolRowViewIterator` / `ConstBoolRowViewIterator`:
 *   lightweight forward iterators for iterating one logical row of
 *   `Matrix<bool>`, yielding `BoolRef` (mutable) or `bool` (const-style).
 *
 * These iterators are single-pass / forward iterators intended for simple
 * range-style loops, not random access.
 */

#include <cstdint>
#include <tuple>
#include <utility>
#include "export.hpp"
#include "matrix.hpp"
#include "rowView.hpp"
#include "boolRef.hpp"
#include "traversal.hpp"

namespace bml {

    /**
     * @brief Helper alias that resolves to the reference/proxy type returned by `Matrix<T>::operator[][]` for writeable access.
     *
     * For most `T` this will be `T&`.
     * For `Matrix<bool>` this is `BoolRef`.
     *
     * @tparam T matrix element type
     */
    template<typename T>
    using element_ref_t =
        decltype(std::declval<Matrix<T>&>()[0][0]); // e.g. T& or BoolRef

    /**
     * @brief Helper alias that resolves to the access type when reading from a `const Matrix<T>`.
     *
     * For most `T` this will be `const T&`.
     * For `Matrix<bool>` this will be `bool` (because BoolRef collapses to bool in const context).
     *
     * @tparam T matrix element type
     */
    template<typename T>
    using const_element_t =
        decltype(std::declval<const Matrix<T>&>()[0][0]); // e.g. const T& or bool


    /**
     * @brief Read-only forward iterator over a `Matrix<T>`.
     *
     * The iterator walks the matrix in the order given by `TraversalType`
     * (typically row-major or column-major).
     *
     * Dereferencing (`operator*`) returns a 3-tuple:
     *
     *   `(row_index, col_index, const_element)`
     *
     * where:
     * - `row_index` and `col_index` are `std::uint32_t`
     * - `const_element` is `const_element_t<T>` (`const T&` or `bool`)
     *
     * Iterators are equal if they refer to the same matrix, same position,
     * and same traversal mode.
     *
     * @tparam T matrix element type
     */
    template<typename T>
    class ConstMatrixIterator
    {
    private:
        /**
         * @brief Reference to the matrix being iterated.
         *
         * Lifetime note: caller must ensure `matrix` outlives this iterator.
         * The iterator does not own the matrix.
         */
        const Matrix<T>& matrix;

        /**
         * @brief Current row position within the matrix.
         *
         * Stored as `std::int64_t` for internal stepping/sentinels.
         * Exposed as `std::uint32_t` via `operator*()`.
         */
        std::int64_t row;

        /**
         * @brief Current column position within the matrix.
         *
         * Stored as `std::int64_t` for internal stepping/sentinels.
         * Exposed as `std::uint32_t` via `operator*()`.
         */
        std::int64_t col;

        /**
         * @brief Traversal order (row-major, column-major, etc.).
         */
        TraversalType type;

    public:
        /**
         * @brief Construct a const iterator at a given matrix position.
         *
         * Usually created by `Matrix<T>::cbegin()` / `cend()`.
         *
         * @param mat Reference to the matrix to iterate.
         * @param r   Starting row index (internal signed form).
         * @param c   Starting column index (internal signed form).
         * @param traversalType Traversal order to use (default: row-major).
         */
        ConstMatrixIterator(const Matrix<T>& mat,
                            std::int64_t r,
                            std::int64_t c,
                            TraversalType traversalType = TraversalType::Row);

        /**
         * @brief Pre-increment.
         *
         * Advances to the next logical element under the chosen `TraversalType`.
         * After the last element, the iterator moves to the `end()` sentinel.
         *
         * @return reference to this iterator after increment
         */
        ConstMatrixIterator& operator++();

        /**
         * @brief Equality comparison.
         *
         * @param other Iterator to compare with.
         * @return true if both iterators point to the same matrix position
         *         with the same traversal mode.
         */
        bool operator==(const ConstMatrixIterator& other) const;

        /**
         * @brief Inequality comparison.
         *
         * @param other Iterator to compare with.
         * @return true if iterators differ; false otherwise.
         */
        bool operator!=(const ConstMatrixIterator& other) const;

        /**
         * @brief Dereference.
         *
         * Returns `(row, col, value)` where:
         * - `row` and `col` are `std::uint32_t`
         * - `value` is `const_element_t<T>` (read-only view of the element)
         *
         * @return tuple of (row, col, value)
         */
        std::tuple<std::uint32_t, std::uint32_t, const_element_t<T>> operator*() const;
    };


    /**
     * @brief Mutable forward iterator over a `Matrix<T>`.
     *
     * Similar to `ConstMatrixIterator<T>`, but dereferencing gives you a
     * writable reference/proxy to the element.
     *
     * Dereferencing (`operator*`) returns:
     *
     *   `(row_index, col_index, element_ref)`
     *
     * where `element_ref` is `element_ref_t<T>`:
     * - `T&` for normal matrices.
     * - `BoolRef` for `Matrix<bool>`.
     *
     * @tparam T matrix element type
     */
    template<typename T>
    class MatrixIterator
    {
    private:
        /**
         * @brief Reference to the matrix being iterated.
         *
         * Lifetime note: caller must ensure `matrix` outlives this iterator.
         */
        Matrix<T>& matrix;

        /**
         * @brief Current row position within the matrix (internal signed form).
         */
        std::int64_t row;

        /**
         * @brief Current column position within the matrix (internal signed form).
         */
        std::int64_t col;

        /**
         * @brief Traversal order (row-major, column-major, etc.).
         */
        TraversalType type;

    public:
        /**
         * @brief Construct a mutable iterator at a given matrix position.
         *
         * Usually created by `Matrix<T>::begin()` / `end()`.
         *
         * @param mat Matrix to iterate.
         * @param r   Starting row index (internal signed form).
         * @param c   Starting column index (internal signed form).
         * @param traversalType Traversal order to use (default: row-major).
         */
        MatrixIterator(Matrix<T>& mat,
                       std::int64_t r,
                       std::int64_t c,
                       TraversalType traversalType = TraversalType::Row);

        /**
         * @brief Pre-increment.
         *
         * Moves to the next element according to `type`.
         *
         * @return reference to this iterator after increment
         */
        MatrixIterator& operator++();

        /**
         * @brief Equality comparison.
         *
         * @param other Iterator to compare with.
         * @return true if both refer to the same logical position.
         */
        bool operator==(const MatrixIterator& other) const;

        /**
         * @brief Inequality comparison.
         *
         * @param other Iterator to compare with.
         * @return true if iterators differ; false otherwise.
         */
        bool operator!=(const MatrixIterator& other) const;

        /**
         * @brief Dereference.
         *
         * Returns `(row, col, ref)` where:
         * - `row` and `col` are `std::uint32_t`
         * - `ref` is `element_ref_t<T>` (writable reference/proxy)
         *
         * @return tuple of (row, col, reference/proxy)
         */
        std::tuple<std::uint32_t, std::uint32_t, element_ref_t<T>> operator*() const;
    };


    /**
     * @brief Forward iterator for a `RowView<bool>` that yields `BoolRef`.
     *
     * Walks across a logical row of a `Matrix<bool>`. Dereferencing gives a
     * writable `BoolRef` proxy to the current element.
     *
     * Single-pass / forward-only.
     */
    class BoolRowViewIterator {
    public:
        /**
         * @brief Construct iterator at the start of a `RowView<bool>`.
         *
         * @param row_view RowView<bool> to iterate.
         *
         * Lifetime note: `row_view` must outlive this iterator.
         */
        explicit BoolRowViewIterator(RowView<bool>& row_view);

    private:
        /**
         * @brief Reference to the underlying row view.
         */
        RowView<bool>& rowView;

        /**
         * @brief Current index within the row.
         *
         * Starts at 0 and increases with `operator++()`.
         */
        std::uint32_t i = 0;

    public:
        /**
         * @brief Dereference.
         *
         * @return BoolRef proxy for the current element (read/write).
         */
        BoolRef operator*() const;

        /**
         * @brief Pre-increment.
         *
         * Advances to the next element in the row.
         *
         * @return reference to this iterator after increment
         */
        BoolRowViewIterator& operator++();

        /**
         * @brief Inequality comparison.
         *
         * @param o Iterator to compare with.
         * @return true if iterators differ; false otherwise.
         */
        bool operator!=(const BoolRowViewIterator& o) const;
    };


    /**
     * @brief Forward iterator for a `RowView<bool>` that yields plain `bool`.
     *
     * Const-style variant. Dereferencing returns a plain `bool` value instead
     * of a writable `BoolRef`.
     *
     * Useful for read-only loops over a `Matrix<bool>`.
     */
    class ConstBoolRowViewIterator {
    public:
        /**
         * @brief Construct const iterator at the start of a `RowView<bool>`.
         *
         * @param row_view RowView<bool> to iterate.
         *
         * Note: currently takes non-const RowView<bool>&.
         * If you later add `const RowView<bool>`, update this.
         */
        explicit ConstBoolRowViewIterator(RowView<bool>& row_view);

    private:
        /**
         * @brief Reference to the underlying row view.
         */
        RowView<bool>& rowView;

        /**
         * @brief Current index within the row.
         */
        std::uint32_t i = 0;

    public:
        /**
         * @brief Dereference.
         *
         * @return bool value of the current element.
         */
        bool operator*() const;

        /**
         * @brief Pre-increment.
         *
         * Advances to the next element in the row.
         *
         * @return reference to this iterator after increment
         */
        ConstBoolRowViewIterator& operator++();

        /**
         * @brief Inequality comparison.
         *
         * @param o Iterator to compare with.
         * @return true if iterators differ; false otherwise.
         */
        bool operator!=(const ConstBoolRowViewIterator& o) const;
    };

} // namespace bml

#endif // BML_ITERATOR_HPP
