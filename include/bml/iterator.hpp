#ifndef ITERATOR_HPP
#define ITERATOR_HPP
#include "bml/export.hpp"
#include "bml/matrix.hpp"
// Helper aliases: deduce element access types
namespace bml{
    template<typename T>
    using element_ref_t =
        decltype(std::declval<Matrix<T>&>()[0][0]);            // e.g. T& or BoolRef

    template<typename T>
    using const_element_t =
        decltype(std::declval<const Matrix<T>&>()[0][0]);      // e.g. const T& or bool

    template<typename T>
    class ConstMatrixIterator
    {
    private:
        // Store a const reference
        const Matrix<T>& matrix;
        long row;
        long col;
        TraversalType type;

    public:
        // Constructor (declaration only here)
        ConstMatrixIterator(const Matrix<T>& mat,
                      long r,
                      long c,
                      TraversalType traversalType = TraversalType::Row);

        // Pre-increment operator
        ConstMatrixIterator& operator++();

        // Comparison
        bool operator==(const ConstMatrixIterator& other) const;
        bool operator!=(const ConstMatrixIterator& other) const;

        // Dereference
        std::tuple<std::uint32_t, std::uint32_t, const_element_t<T>> operator*() const;
    };
    template<typename T>
    class MatrixIterator
    {
    private:
        Matrix<T>& matrix;
        long row;
        long col;
        TraversalType type;  // Add traversal type

    public:
        MatrixIterator(Matrix<T>& mat, std::int64_t r, std::int64_t c, TraversalType traversalType = TraversalType::Row);

        MatrixIterator& operator++();
        bool operator==(const MatrixIterator& other) const;
        bool operator!=(const MatrixIterator& other) const;
        std::tuple<std::uint32_t, std::uint32_t, element_ref_t<T>> operator*() const;
    };

    class BoolRowViewIterator {
    public:
        explicit BoolRowViewIterator(RowView<bool>& row_view);


    private:
        RowView<bool>& rowView;
        std::uint32_t i=0;

    public:
        BoolRef operator*() const;
        BoolRowViewIterator& operator++();
        bool operator!=(const BoolRowViewIterator& o) const;
    };

    class ConstBoolRowViewIterator {
    public:
        explicit ConstBoolRowViewIterator(RowView<bool>& row_view);

    private:
        RowView<bool>& rowView;
        std::uint32_t i=0;

    public:
        bool operator*() const;
        ConstBoolRowViewIterator& operator++();
        bool operator!=(const ConstBoolRowViewIterator& o) const;
    };
}
#endif // ITERATOR_HPP
