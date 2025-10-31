#ifndef ITERATOR_HPP
#define ITERATOR_HPP
#include "bml/export.hpp"
#include "bml/matrix.hpp"

// Helper aliases: deduce element access types

namespace bml{

    /**
     * @brief Helper alias that resolves to the reference type returned by Matrix<T>::operator[][]
     *
     * For most T this will be `T&`. For Matrix<bool> this will typically be BoolRef.
     *
     * @tparam T matrix element type
     */
    template<typename T>
    using element_ref_t =
        decltype(std::declval<Matrix<T>&>()[0][0]);            // e.g. T& or BoolRef

    /**
     * @brief Helper alias that resolves to the const access type of Matrix<T>::operator[][]
     *
     * For most T this will be `const T&`. For Matrix<bool> this will be `bool`
     * (because BoolRef collapses to bool in const context).
     *
     * @tparam T matrix element type
     */
    template<typename T>
    using const_element_t =
        decltype(std::declval<const Matrix<T>&>()[0][0]);      // e.g. const T& or bool


    /**
     * @brief Read-only forward iterator over a Matrix<T>.
     *
     * This iterator walks a Matrix<T> in either row-major or column-major
     * order, depending on the @ref TraversalType provided at construction.
     *
     * Dereferencing (`operator*`) returns a 3-tuple:
     *   (row index, column index, const element reference/value).
     *
     * The row/column indices are always returned as std::uint32_t.
     * The element part uses const_element_t<T>, so `const T&` for normal
     * matrices and `bool` for Matrix<bool>.
     *
     * The iterator is considered equal to another iterator if they refer
     * to the same underlying Matrix<T>, same coordinates, and same
     * traversal mode.
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
         * Lifetime note: the caller must ensure that @ref matrix outlives
         * the iterator. The iterator does not take ownership.
         */
        const Matrix<T>& matrix;

        /**
         * @brief Current row position within the matrix.
         *
         * Stored as signed long for internal stepping/sentinels,
         * but exposed as std::uint32_t in operator*().
         */
        long row;

        /**
         * @brief Current column position within the matrix.
         *
         * Stored as signed long for internal stepping/sentinels,
         * but exposed as std::uint32_t in operator*().
         */
        long col;

        /**
         * @brief Traversal order (row-major or column-major).
         */
        TraversalType type;

    public:
        /**
         * @brief Construct a const iterator at a given (row, col) in a matrix.
         *
         * Usually created by Matrix<T>::cbegin() / cend() helpers.
         *
         * @param mat const Matrix<T>&  Matrix to iterate
         * @param r   long              Starting row position
         * @param c   long              Starting column position
         * @param traversalType TraversalType  Row-major or Column-major
         */
        ConstMatrixIterator(const Matrix<T>& mat,
                            long r,
                            long c,
                            TraversalType traversalType = TraversalType::Row);

        /**
         * @brief Pre-increment operator.
         *
         * Advances the iterator to the next logical element according
         * to the chosen TraversalType. After the last element, the
         * iterator typically moves to a sentinel state comparable to `end()`.
         *
         * @return ConstMatrixIterator& reference to this iterator after increment
         */
        ConstMatrixIterator& operator++();

        /**
         * @brief Compare two const iterators for equality.
         *
         * @param other const ConstMatrixIterator&  iterator to compare with
         * @return true if both iterators point to the same matrix position
         *         (including traversal mode); false otherwise
         */
        bool operator==(const ConstMatrixIterator& other) const;

        /**
         * @brief Compare two const iterators for inequality.
         *
         * @param other const ConstMatrixIterator& iterator to compare with
         * @return true if iterators differ; false otherwise
         */
        bool operator!=(const ConstMatrixIterator& other) const;

        /**
         * @brief Dereference operator.
         *
         * Returns a tuple of:
         *   ( row index, column index, const element reference/value )
         *
         * The indices are std::uint32_t, so they are always non-negative.
         *
         * @return std::tuple<std::uint32_t, std::uint32_t, const_element_t<T>>
         *         triple of (row, col, value)
         */
        std::tuple<std::uint32_t, std::uint32_t, const_element_t<T>> operator*() const;
    };

    /**
     * @brief Mutable forward iterator over a Matrix<T>.
     *
     * This works like ConstMatrixIterator<T> but allows modifying elements.
     *
     * Dereferencing (`operator*`) returns:
     *   (row index, column index, mutable element reference/proxy)
     *
     * For most T, that element reference is `T&`.
     * For Matrix<bool> it is BoolRef, which behaves like a writable bool proxy.
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
         * Lifetime note: caller must ensure @ref matrix outlives this iterator.
         */
        Matrix<T>& matrix;

        /**
         * @brief Current row position within the matrix.
         *
         * Stored as long for internal sentinel handling.
         */
        long row;

        /**
         * @brief Current column position within the matrix.
         *
         * Stored as long for internal sentinel handling.
         */
        long col;

        /**
         * @brief Traversal order (row-major or column-major).
         */
        TraversalType type;

    public:
        /**
         * @brief Construct a mutable iterator at a given (row, col).
         *
         * Usually created by Matrix<T>::begin() / end() helpers.
         *
         * @param mat Matrix<T>&        Matrix to iterate
         * @param r   std::int64_t      Starting row position
         * @param c   std::int64_t      Starting column position
         * @param traversalType TraversalType Row-major or Column-major
         */
        MatrixIterator(Matrix<T>& mat,
                       std::int64_t r,
                       std::int64_t c,
                       TraversalType traversalType = TraversalType::Row);

        /**
         * @brief Pre-increment operator.
         *
         * Steps to the next element according to @ref type.
         *
         * @return MatrixIterator& reference to this iterator after increment
         */
        MatrixIterator& operator++();

        /**
         * @brief Equality comparison.
         *
         * @param other const MatrixIterator& iterator to compare
         * @return true if both iterators refer to the same logical position;
         *         false otherwise
         */
        bool operator==(const MatrixIterator& other) const;

        /**
         * @brief Inequality comparison.
         *
         * @param other const MatrixIterator& iterator to compare
         * @return true if iterators differ; false otherwise
         */
        bool operator!=(const MatrixIterator& other) const;

        /**
         * @brief Dereference operator.
         *
         * Returns a tuple of:
         *   ( row index, column index, writable reference/proxy to element )
         *
         * The element reference type is deduced by element_ref_t<T>.
         *
         * @return std::tuple<std::uint32_t, std::uint32_t, element_ref_t<T>>
         *         triple of (row, col, reference/proxy)
         */
        std::tuple<std::uint32_t, std::uint32_t, element_ref_t<T>> operator*() const;
    };

    /**
     * @brief Forward iterator over a RowView<bool> that yields BoolRef.
     *
     * This iterator walks across a RowView<bool> (a single logical row of a
     * Matrix<bool>). Dereferencing returns a BoolRef proxy so the caller
     * can both read and write each boolean cell.
     *
     * The iterator is single-pass / forward only.
     */
    class BoolRowViewIterator {
    public:
        /**
         * @brief Construct iterator at the start of a RowView<bool>.
         *
         * @param row_view RowView<bool>&  reference to the row to iterate
         *
         * Lifetime note: The RowView<bool> must outlive the iterator.
         */
        explicit BoolRowViewIterator(RowView<bool>& row_view);

    private:
        /**
         * @brief Reference to the underlying RowView<bool>.
         */
        RowView<bool>& rowView;

        /**
         * @brief Current index within the row.
         *
         * Starts at 0 and increases with operator++().
         */
        std::uint32_t i = 0;

    public:
        /**
         * @brief Dereference operator.
         *
         * Returns a BoolRef proxy to the current bit/byte in the row.
         * Can be assigned to (e.g. `*it = true;`).
         *
         * @return BoolRef proxy reference to current element
         */
        BoolRef operator*() const;

        /**
         * @brief Pre-increment.
         *
         * Moves to the next boolean in the row.
         *
         * @return BoolRowViewIterator& reference to this iterator after increment
         */
        BoolRowViewIterator& operator++();

        /**
         * @brief Inequality comparison.
         *
         * Iterators are considered different if they point to different
         * element indices or different RowView<bool> objects.
         *
         * @param o const BoolRowViewIterator& iterator to compare
         * @return true if different; false otherwise
         */
        bool operator!=(const BoolRowViewIterator& o) const;
    };

    /**
     * @brief Forward iterator over a RowView<bool> that yields plain bool.
     *
     * This is the const variant. Dereferencing returns a bool value,
     * not a writable BoolRef. Useful for read-only loops over Matrix<bool>.
     */
    class ConstBoolRowViewIterator {
    public:
        /**
         * @brief Construct const iterator at the start of a RowView<bool>.
         *
         * @param row_view RowView<bool>&  reference to the row to iterate
         *
         * Note: row_view is non-const here; if you later make a const
         * RowView<bool> type you might want to update this signature.
         */
        explicit ConstBoolRowViewIterator(RowView<bool>& row_view);

    private:
        /**
         * @brief Reference to the underlying RowView<bool>.
         */
        RowView<bool>& rowView;

        /**
         * @brief Current index within the row.
         */
        std::uint32_t i = 0;

    public:
        /**
         * @brief Dereference operator.
         *
         * Returns the current boolean value as a plain bool.
         *
         * @return bool value of the current element
         */
        bool operator*() const;

        /**
         * @brief Pre-increment.
         *
         * Moves to the next element in the row.
         *
         * @return ConstBoolRowViewIterator& reference to this iterator after increment
         */
        ConstBoolRowViewIterator& operator++();

        /**
         * @brief Inequality comparison.
         *
         * @param o const ConstBoolRowViewIterator& iterator to compare
         * @return true if different; false otherwise
         */
        bool operator!=(const ConstBoolRowViewIterator& o) const;
    };
}
#endif // ITERATOR_HPP
