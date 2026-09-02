#ifndef BML_MATRIX_HPP
#define BML_MATRIX_HPP

#include "export.hpp"
#include "typeTraits.hpp"
#include "rowView.hpp"
#include "traversal.hpp"

#include <functional>
#include <string>
#include <type_traits>
#include <vector>

namespace bml
{
    template <class T>
    class MatrixIterator;
    template <class T>
    class ConstMatrixIterator;


    /**
     * @brief Matrix class. Internaly stores values flat, but present them as 2d.
    * @tparam T can be bool, {u,}int{8,16,32,64}_t, "
            "float/double/long double, char, std::string)"
     */
    template <typename T>
    class BML_API Matrix
    {
        // Fail fast for completely unsupported storage types.
        static_assert(
            bml_is_bool<T>::value || bml_is_math_arithmetic<T>::value
            || std::is_same_v<T, char> || std::is_same_v<T, std::string>,
            "bml::Matrix<T>: unsupported T (allowed: bool, {u,}int{8,16,32,64}_t, "
            "float/double/long double, char, std::string)"
        );

    private:
        /**
         * @brief Underlying storage type used for matrix elements.
         *
         * This is derived from the template parameter @c T using @c storage_of_t
         * and may differ from @c T for optimisation or proxy purposes.
         */
        using store_t = storage_of_t<T>;

        /**
         * @brief Contiguous storage for all matrix elements.
         *
         * Elements are stored in row-major order using the underlying @c store_t type.
         */
        std::vector<store_t> data;

        /**
         * @brief Number of rows in the matrix.
         */
        std::uint32_t rows;

        /**
         * @brief Number of columns in the matrix.
         */
        std::uint32_t cols;

        /**
         * @brief Converts 2D coordinates to a linear index.
         *
         * @param r Zero-based row index.
         * @param c Zero-based column index.
         * @return Linear index into @c data corresponding to (r, c).
         *
         * @note Assumes @p r and @p c are within valid bounds.
         */
        [[nodiscard]] std::size_t toIdx(std::uint32_t r, std::uint32_t c) const noexcept;

        /**
         * @brief Converts a linear index to 2D matrix coordinates.
         *
         * @param i Zero-based linear index into @c data.
         * @return A pair (row, column) corresponding to @p i.
         *
         * @note Assumes @p i is within valid bounds.
         */
        [[nodiscard]] std::pair<std::uint32_t, std::uint32_t> toCoords(std::size_t i) const;


    public:
        /**
         * @brief Constructs a matrix with the given dimensions.
         *
         * All elements are value-initialised (e.g. to zero for arithmetic types).
         *
         * @param numRows Number of rows in the matrix.
         * @param numCols Number of columns in the matrix.
         */
        Matrix(std::uint32_t numRows, std::uint32_t numCols);

        /**
         * @brief Copy constructor.
         *
         * Creates a new matrix as an exact copy of @p other.
         *
         * @param other Matrix to copy from.
         */
        Matrix(const Matrix& other) = default;

        /**
         * @brief Copy assignment operator.
         *
         * Replaces the contents of this matrix with a copy of @p other.
         *
         * @param other Matrix to copy from.
         * @return Reference to this matrix.
         */
        Matrix& operator=(const Matrix& other);

        /**
         * @brief Move constructor.
         *
         * Constructs a matrix by taking ownership of the contents of @p other.
         * @param other Matrix to move from.
         */
        Matrix(Matrix&& other) noexcept;

        /**
         * @brief Move assignment operator.
         *
         * Replaces the contents of this matrix by taking ownership of @p other.
         * @param other Matrix to move from.
         * @return Reference to this matrix.
         */
        Matrix& operator=(Matrix&& other) noexcept;

        /**
         * @brief Destructor.
         */
        ~Matrix() = default;

        
        /**
         * @brief Returns the number of rows in the matrix
         * @return The number of rows
         */
        [[nodiscard]] std::uint32_t numRows() const;


        /**
         * @brief Returns the number of columns in the Matrix
         * @return The number of columns
         */
        [[nodiscard]] std::uint32_t numCols() const;


        /**
         * @brief Returns the number of elements in the Matrix
         * @return The number of elements
         */
        [[nodiscard]] size_t size() const noexcept;


        /**
         * @brief Returns true if the Matrix has zero dims
         * @return
         */
        [[nodiscard]] bool empty() const noexcept;

        /**
 * @brief Checks whether any element satisfies the predicate.
 *
 * Iterates elements in row-major order and short-circuits on the first @c true.
 *
 * @param p Unary predicate invoked as @c p(element).
 * @return @c true if at least one element makes @p p return @c true; otherwise @c false.
 * @note Complexity: O(rows*cols) in the worst case.
 * @note Exceptions: any exception thrown by @p p propagates.
 */
        bool any_of(std::function<bool(T)> p) const;

        /**
         * @brief Checks whether no element satisfies the predicate.
         *
         * Iterates elements in row-major order and returns on the first @c true from @p p.
         *
         * @param p Unary predicate invoked as @c p(element).
         * @return @c true if @p p returns @c true for no elements; otherwise @c false.
         * @note Complexity: O(rows*cols) in the worst case.
         * @note Exceptions: any exception thrown by @p p propagates.
         */
        bool none_of(std::function<bool(T)> p) const;


        /**
 * @brief Row view accessor (non-owning).
 *
 * @param row Zero-based row index.
 * @return @ref bml::RowView<T> — see its docs for semantics, lifetime, and caveats.
 * @see bml::RowView, bml::RowView<const bool>, bml::BoolRef
 */
        RowView<T> operator[](std::uint32_t row);

        /**
         * @overload
         * @return @ref bml::RowView<const T> — see @ref bml::RowView for details.
         */
        RowView<const T> operator[](std::uint32_t row) const;


        /**
         * @brief init the matrix's values from a bytestream.
         * @param byteStream uint8_t (char)* bytestream
         * @param byteSize size_t length of byte stream
         */
        void initFromByteStream(const std::uint8_t* byteStream, size_t byteSize);


        /**
         * @see bml::matrix::initFromByteStream
         * @param byteStream std::vector<uint8_t>
         */
        void initFromByteStream(const std::vector<uint8_t>& byteStream);


        /**
         * @brief Serialises the values of the matrix into a bytestream.
         * @note String is serialised using @see StringStorage
         * @return a bytestream of the matrix inside a vector
         */
        [[nodiscard]] std::vector<std::uint8_t> toByteStream() const;



        /**
         * @brief Returns a copy of the matrix or a rectangular sub-matrix.
         *
         * The region is defined by a start coordinate and an end coordinate.
         * The @p endRow / @p endCol parameters are **inclusive**. A value of -1
         * means “use the last row/column”.
         *
         * @param startRow Zero-based start row (default: 0).
         * @param startCol Zero-based start column (default: 0).
         * @param endRow   Inclusive end row, or -1 for the last row (default: -1).
         * @param endCol   Inclusive end column, or -1 for the last column (default: -1).
         * @return A new Matrix containing the selected region, with
         *         dimensions (endRow - startRow + 1) × (endCol - startCol + 1).
         *
         * @pre startRow < numRows(), startCol < numCols().
         * @pre After resolving -1 sentinels: endRow >= startRow and endCol >= startCol.
         *
         * @throws std::out_of_range    If any (resolved) index is outside the matrix bounds.
         * @throws std::invalid_argument If start exceeds end on either axis.
         *
         * @note Complexity: O(region_rows × region_cols).
         * @note Elements are copied in row-major order. For @c T==bool, copies use the
         *       byte-backed representation via @c BoolRef reads.
         */
        Matrix copy(std::uint32_t startRow = 0, std::uint32_t startCol = 0,
                    std::int64_t endRow = -1, std::int64_t endCol = -1) const;



        /**
 * @brief Pastes @p source into this matrix at (destRow, destCol).
 *
 * @param source   Matrix to copy from.
 * @param destRow  Zero-based destination start row (default: 0).
 * @param destCol  Zero-based destination start column (default: 0).
 *
 * @pre destRow < numRows(), destCol < numCols().
 * @pre source.numRows() <= numRows() - destRow
 * @pre source.numCols() <= numCols() - destCol
 *
 * @throws std::out_of_range If the pasted region would exceed this matrix.
 *
 * @note Elements are copied in row-major order. For @c T==bool, assignment
 *       writes 0/1 bytes via the internal representation.
 */
        void paste(const Matrix& source, std::uint32_t destRow = 0, std::uint32_t destCol = 0);

        /**
         * @brief Returns true if @p condition holds for every element.
         *
         * Iterates row-major and short-circuits on the first @c false.
         *
         * @param condition Unary predicate called as @c condition(element).
         * @return @c true iff the predicate returns @c true for all elements.
         *
         * @note Complexity: O(rows*cols) worst-case. Exceptions thrown by @p condition propagate.
         */
        bool all(std::function<bool(T)> condition) const;

        /**
         * @brief Element-wise conditional selection.
         *
         * For each element @c x, applies @p condition(x); if @c true, the result cell
         * receives @p trueValue, otherwise @p falseValue.
         *
         * @param condition Unary predicate called as @c condition(element).
         * @param trueValue  Value to store when the predicate is @c true.
         * @param falseValue Value to store when the predicate is @c false.
         * @return A new matrix with the same shape as this one.
         *
         * @note Complexity: O(rows*cols). Exceptions from @p condition propagate.
         */
        Matrix<T> where(std::function<bool(T)> condition, T trueValue, T falseValue) const;


            /**
     * @brief Return an iterator to the first element of the matrix.
     *
     * Iteration order is controlled by @p type:
     *  - TraversalType::Row    = row-major (row 0, col 0..N; then row 1, ...)
     *  - TraversalType::Column = column-major (col 0, row 0..M; then col 1, ...)
     *
     * The returned iterator is mutable: dereferencing yields a writable
     * proxy/reference to the element (for most T this is `T&`, for `bool`
     * it is BoolRef). See @ref MatrixIterator "MatrixIterator<T>".
     *
     * @param type TraversalType  desired traversal order (default is row-major)
     * @return MatrixIterator<T> iterator positioned at the first logical element
     *
     * @see Matrix::end()
     * @see MatrixIterator
     */
    MatrixIterator<T> begin(TraversalType type = TraversalType::Row);

    /**
     * @brief Return a past-the-end iterator for this matrix.
     *
     * The returned iterator compares equal to any iterator that has been
     * incremented past the last valid element for the same @p type.
     *
     * This is meant to be used together with Matrix::begin() in range-style
     * loops:
     *
     * @code
     * for (auto it = mat.begin(); it != mat.end(); ++it) {
     *     auto [r, c, cell] = *it;
     *     // modify cell here
     * }
     * @endcode
     *
     * See @ref MatrixIterator "MatrixIterator<T>".
     *
     * @param type TraversalType  traversal order that matches the begin() you use
     * @return MatrixIterator<T> sentinel / past-the-end iterator
     *
     * @see Matrix::begin()
     * @see MatrixIterator
     */
    MatrixIterator<T> end(TraversalType type = TraversalType::Row);

    /**
     * @brief Return a const iterator to the first element of the matrix.
     *
     * This overload is for read-only access. Dereferencing yields a tuple
     * `(row, col, value)` where `value` is `const T&` for normal matrices
     * and `bool` for Matrix<bool>. See
     * @ref ConstMatrixIterator "ConstMatrixIterator<T>".
     *
     * @param type TraversalType  desired traversal order (default is row-major)
     * @return ConstMatrixIterator<T> const iterator positioned at the first element
     *
     * @see Matrix::end() const
     * @see ConstMatrixIterator
     */
    ConstMatrixIterator<T> begin(TraversalType type = TraversalType::Row) const;

    /**
     * @brief Return a const past-the-end iterator for this matrix.
     *
     * Works like the non-const Matrix::end(), but returns a read-only
     * iterator. Suitable for const Matrix instances and for algorithms
     * that should not modify elements.
     *
     * See @ref ConstMatrixIterator "ConstMatrixIterator<T>".
     *
     * @param type TraversalType  traversal order that matches the const begin() you use
     * @return ConstMatrixIterator<T> const sentinel / past-the-end iterator
     *
     * @see Matrix::begin() const
     * @see ConstMatrixIterator
     */
    ConstMatrixIterator<T> end(TraversalType type = TraversalType::Row) const;


        /**
  * @brief Convert the entire matrix to a human-readable string.

  *
  * @return std::string  textual representation of the matrix contents
  */
        [[nodiscard]] std::string toString() const;


        /**
         * @brief Extract a horizontal slice of a row as a std::vector.
         *
         * Copies elements from the given row index @p row, starting at column
         * @p startCol and ending at @p endCol (inclusive).
         *
         * Special case: if @p endCol is -1 (default), the slice continues to the
         * final valid column in this row.
         *
         * This function validates bounds and will throw if:
         *  - @p row is outside [0, rows-1], or
         *  - @p startCol / @p endCol (after resolving -1) are outside [0, cols-1], or
         *  - @p startCol > @p endCol.
         *
         * @param row std::uint32_t   which row to read (0-based)
         * @param startCol std::int32_t first column to include (0-based, default 0)
         * @param endCol std::int32_t   last column to include (inclusive).
         *                              -1 means "use last column"
         * @return std::vector<T>  copy of the requested row segment in left-to-right order
         *
         * @throws if the requested slice is out of bounds or invalid
         */
        std::vector<T> getRow(std::uint32_t row,
                              std::int32_t startCol = 0,
                              std::int32_t endCol = -1) const;

        /**
         * @brief Extract a vertical slice of a column as a std::vector.
         *
         * Copies elements from the given column index @p col, starting at row
         * @p startRow and ending at @p endRow (inclusive).
         *
         * Special case: if @p endRow is -1 (default), the slice continues to the
         * final valid row in this column.
         *
         * This function validates bounds and will throw if:
         *  - @p col is outside [0, cols-1], or
         *  - @p startRow / @p endRow (after resolving -1) are outside [0, rows-1], or
         *  - @p startRow > @p endRow.
         *
         * @param col std::uint32_t     which column to read (0-based)
         * @param startRow std::int32_t first row to include (0-based, default 0)
         * @param endRow std::int32_t   last row to include (inclusive).
         *                              -1 means "use last row"
         * @return std::vector<T>  copy of the requested column segment in top-to-bottom order
         *
         * @throws if the requested slice is out of bounds or invalid
         */
        std::vector<T> getColumn(std::uint32_t col,
                                 std::int32_t startRow = 0,
                                 std::int32_t endRow = -1) const;

        /**
         * @brief Extract the main diagonal (top-left → bottom-right) as a std::vector.
         *
         * Returns elements where row == col.
         *
         * The optional @p start and @p end parameters select a subrange of that
         * diagonal. These indices are positions along the diagonal itself
         * (0 == element (0,0), 1 == element (1,1), …), not absolute row/col.
         *
         * Special case: if @p end is -1 (default), the slice continues to the
         * last valid diagonal element (i.e. min(rows, cols) - 1).
         *
         * This function validates bounds and will throw if:
         *  - @p start is negative or beyond the last diagonal index, or
         *  - @p end (after resolving -1) is beyond the last diagonal index, or
         *  - @p start > @p end.
         *
         * @param start std::int32_t first diagonal index to include (default 0)
         * @param end   std::int32_t last diagonal index to include (inclusive).
         *                            -1 means "use last diagonal element"
         * @return std::vector<T> elements of the main diagonal in order
         *
         * @throws if the requested range is out of bounds or invalid
         */
        std::vector<T> getDiagonal(std::int32_t start = 0,
                                   std::int32_t end = -1) const;

        /**
         * @brief Extract the anti-diagonal (top-right → bottom-left) as a std::vector.
         *
         * Returns elements where:
         *   col = (last column - row)
         * i.e. (row 0, last col), (row 1, last col-1), …
         *
         * The optional @p start and @p end parameters index along that
         * anti-diagonal. Index 0 is the top-right corner element, index 1 is
         * the next one down-left, and so on.
         *
         * Special case: if @p end is -1 (default), the slice continues to the
         * last valid anti-diagonal element within the matrix bounds.
         *
         * This function validates bounds and will throw if:
         *  - @p start is negative or beyond the last anti-diagonal index, or
         *  - @p end (after resolving -1) is beyond the last anti-diagonal index, or
         *  - @p start > @p end.
         *
         * @param start std::int32_t first anti-diagonal index to include (default 0)
         * @param end   std::int32_t last anti-diagonal index to include (inclusive).
         *                            -1 means "use last anti-diagonal element"
         * @return std::vector<T> elements of the anti-diagonal in order
         *
         * @throws if the requested range is out of bounds or invalid
         */
        std::vector<T> getAntiDiagonal(std::int32_t start = 0,
                                       std::int32_t end = -1) const;


        /**
 * @brief Overwrite every element in the matrix with a single value.
 *
 * After this call, all cells in the matrix will compare equal to @p value.
 *
 * @param value const T&  the value to assign to every element
 */
        void fill(const T& value);


         // --- Reductions (SFINAE-gated) ---

        /**
         * @brief Sum of all elements in the matrix (floating point version).
         *
         * This overload is only enabled if T is a floating-point type
         * (e.g. float, double, long double).
         *
         * The implementation uses a compensated summation (Kahan-style)
         * to reduce the effect of rounding error when accumulating many
         * floating-point values.
         *
         * Behaviour:
         *  - For an empty matrix (rows * cols == 0), the return value is 0 of type T.
         *  - Otherwise returns the arithmetic sum of all cells.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return T  the sum of all elements
         *
         * @see sum() const (integral / non-floating version)
         */
        template <typename U = T>
        std::enable_if_t<std::is_floating_point<U>::value, T>
        sum() const;

        /**
         * @brief Sum of all elements in the matrix (integer / non-floating arithmetic).
         *
         * This overload is enabled if T is considered arithmetic for this library
         * (bml_is_math_arithmetic<T> true) but is NOT a floating-point type.
         * Typical examples: std::int32_t, std::uint64_t, char, unsigned char, etc.
         *
         * Behaviour:
         *  - For an empty matrix, the return value is 0 of type T.
         *  - Otherwise returns the arithmetic sum using normal addition.
         *
         * Note: No overflow protection is performed. If T cannot hold the true
         * mathematical sum, it will wrap in the usual C++ way.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return T  the sum of all elements
         *
         * @see sum() const (floating-point version)
         */
        template <typename U = T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value && !std::is_floating_point<U>::value, T>
        sum() const;

        /**
         * @brief Deleted sum() for unsupported element types.
         *
         * If T is not arithmetic according to bml_is_math_arithmetic<T>,
         * this overload is selected and is =delete, causing a compile-time
         * error instead of allowing accidental summation of e.g. std::string,
         * structs, pointers, etc.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         */
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, T>
        sum() const = delete;


        /**
         * @brief Return the minimum element in the matrix.
         *
         * Enabled only if T is NOT a pointer type. If T is a pointer,
         * min() is deleted (see below) to prevent accidental min/max
         * on raw pointers.
         *
         * Throws if the matrix is empty.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return T  the smallest element found, using operator<
         *
         * @throws if the matrix has size 0
         *
         * @see min() const = delete (pointer version)
         * @see max()
         * @see argmin()
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value, T>::type
        min() const; // throws on empty

        /**
         * @brief Deleted min() for pointer element types.
         *
         * If T is a pointer type (e.g. int*), this overload is selected and
         * is =delete, which triggers a compile-time error. The goal is to
         * make it explicit that pointer ordering is not defined/meaningful
         * in this API.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         */
        template <typename U = T>
        typename std::enable_if<std::is_pointer<U>::value, T>::type
        min() const = delete;


        /**
         * @brief Return the maximum element in the matrix.
         *
         * Enabled only if T is NOT a pointer type. If T is a pointer,
         * max() is deleted (see below).
         *
         * Throws if the matrix is empty.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return T  the largest element found, using operator<
         *
         * @throws if the matrix has size 0
         *
         * @see max() const = delete (pointer version)
         * @see min()
         * @see argmax()
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value, T>::type
        max() const;

        /**
         * @brief Deleted max() for pointer element types.
         *
         * If T is a pointer type, this overload is selected and =delete,
         * so code like Matrix<int*>::max() will fail at compile time.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         */
        template <typename U = T>
        typename std::enable_if<std::is_pointer<U>::value, T>::type
        max() const = delete;


        /**
         * @brief Return the index of the minimum element in the matrix.
         *
         * Enabled only if T is NOT a pointer type.
         *
         * Scans the matrix for the smallest element (using operator< on T)
         * and returns its coordinates.
         *
         * The returned pair is (row, col), both 0-based.
         *
         * Throws if the matrix is empty.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return std::pair<std::uint32_t, std::uint32_t>
         *         row/column of the first occurrence of the minimum element
         *
         * @throws if the matrix has size 0
         *
         * @see argmin() const = delete (pointer version)
         * @see argmax()
         * @see min()
         */
        template <typename U = T>
        std::enable_if_t<!std::is_pointer<U>::value,
                         std::pair<std::uint32_t, std::uint32_t>>
        argmin() const;

        /**
         * @brief Deleted argmin() for pointer element types.
         *
         * If T is a pointer type, this overload is selected and =delete.
         * We don't define "minimum pointer" semantics in this API.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         */
        template <typename U = T>
        std::enable_if_t<std::is_pointer<U>::value,
                         std::pair<std::uint32_t, std::uint32_t>>
        argmin() const = delete;


        /**
         * @brief Return the index of the maximum element in the matrix.
         *
         * Enabled only if T is NOT a pointer type.
         *
         * Scans the matrix for the largest element (using operator< on T)
         * and returns its coordinates.
         *
         * The returned pair is (row, col), both 0-based.
         *
         * Throws if the matrix is empty.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return std::pair<std::uint32_t, std::uint32_t>
         *         row/column of the first occurrence of the maximum element
         *
         * @throws if the matrix has size 0
         *
         * @see argmax() const = delete (pointer version)
         * @see argmin()
         * @see max()
         */
        template <typename U = T>
        std::enable_if_t<!std::is_pointer<U>::value,
                         std::pair<std::uint32_t, std::uint32_t>>
        argmax() const;

        /**
         * @brief Deleted argmax() for pointer element types.
         *
         * If T is a pointer type, this overload is selected and =delete.
         * As with min()/max(), the API refuses to define ordering on pointers.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         */
        template <typename U = T>
        std::enable_if_t<std::is_pointer<U>::value,
                         std::pair<std::uint32_t, std::uint32_t>>
        argmax() const = delete;


              // ---- Matrix-matrix arithmetic (char/bool excluded; signed/unsigned char allowed) ----

        /**
         * @brief Element-wise addition of two matrices.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T> (that is:
         * numeric types we consider arithmetic, excluding `bool` and plain `char`,
         * but including signed/unsigned char, integers, and floating point).
         *
         * Performs element-wise:
         *     result(r,c) = this(r,c) + other(r,c)
         *
         * Requirements:
         *  - The two matrices must have identical shape (same rows and cols),
         *    otherwise this function throws.
         *
         * @param other const Matrix<T>& matrix to add, same dimensions as *this
         * @return Matrix<T> new matrix holding the per-element sums
         *
         * @throws if matrix dimensions do not match
         *
         * @note If T does not satisfy bml_is_math_arithmetic<T>, this overload
         *       is deleted at compile time (see the =delete variant below).
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator+(const Matrix<T>& other) const;

        /// @copydoc operator+(const Matrix<T>&) const
        /// @note This overload is selected when T is NOT arithmetic by our rules,
        ///       and is deleted to cause a compile-time error.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator+(const Matrix<T>& other) const = delete;


        /**
         * @brief Element-wise subtraction of two matrices.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * result(r,c) = this(r,c) - other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix<T>& matrix to subtract (same shape)
         * @return Matrix<T> per-element (this - other)
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator-(const Matrix<T>& other) const;

        /// See operator-(const Matrix<T>&) const. Deleted if T is not arithmetic.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator-(const Matrix<T>&) const = delete;


        /**
         * @brief Element-wise multiplication of two matrices.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * NOTE: This is NOT matrix multiplication / linear algebra product.
         * It is pointwise:
         *     result(r,c) = this(r,c) * other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix<T>& right-hand matrix (same shape)
         * @return Matrix<T> per-element product
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator*(const Matrix<T>& other) const;

        /// Deleted when T is not arithmetic.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator*(const Matrix<T>&) const = delete;


        /**
         * @brief Element-wise division of two matrices.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * Pointwise:
         *     result(r,c) = this(r,c) / other(r,c)
         *
         * Throws if dimensions differ.
         * Behaviour of division by zero is the normal C++ / hardware behaviour
         * for type T (e.g. UB for integer divide-by-zero, inf/NaN for float).
         *
         * @param other const Matrix<T>& divisor matrix (same shape)
         * @return Matrix<T> per-element quotient
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator/(const Matrix<T>& other) const;

        /// Deleted when T is not arithmetic.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator/(const Matrix<T>&) const = delete;



        // ---- Scalar arithmetic (char/bool excluded; signed/unsigned char allowed) ----

        /**
         * @brief Element-wise matrix + scalar.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * result(r,c) = this(r,c) + scalar
         *
         * @param scalar const T& value to add to each element
         * @return Matrix<T> new matrix after addition
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator+(const T& scalar) const;

        /// Deleted when T is not arithmetic.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator+(const T&) const = delete;


        /**
         * @brief Element-wise matrix - scalar.
         *
         * result(r,c) = this(r,c) - scalar
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * @param scalar const T& value to subtract from each element
         * @return Matrix<T> new matrix after subtraction
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator-(const T& scalar) const;

        /// Deleted when T is not arithmetic.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator-(const T&) const = delete;


        /**
         * @brief Element-wise matrix * scalar.
         *
         * result(r,c) = this(r,c) * scalar
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * @param scalar const T& multiplier
         * @return Matrix<T> new matrix after scaling
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator*(const T& scalar) const;

        /// Deleted when T is not arithmetic.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator*(const T&) const = delete;


        /**
         * @brief Element-wise matrix / scalar.
         *
         * result(r,c) = this(r,c) / scalar
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * Division-by-zero behaviour is whatever C++ does for T.
         *
         * @param scalar const T& divisor
         * @return Matrix<T> new matrix after division
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_arithmetic<U>::value, Matrix<T>>::type
        operator/(const T& scalar) const;

        /// Deleted when T is not arithmetic.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix<T>>
        operator/(const T&) const = delete;



        // ---- Modulus only for "real" integrals (plain char/bool excluded) ----

        /**
         * @brief Element-wise modulus (remainder) of two matrices.
         *
         * Enabled only if T satisfies bml_is_math_integral<T> (an integral type
         * we actually allow for mod). `bool` and plain `char` are excluded.
         *
         * result(r,c) = this(r,c) % other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix<T>& RHS matrix (same shape)
         * @return Matrix<T> per-element remainder
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_integral<U>::value, Matrix<T>>::type
        operator%(const Matrix<T>& other) const;

        /// Deleted when T is not in the allowed integral set.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix<T>>
        operator%(const Matrix<T>&) const = delete;


        /**
         * @brief Element-wise modulus (remainder) with a scalar.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * result(r,c) = this(r,c) % scalar
         *
         * @param scalar const T& RHS value
         * @return Matrix<T> per-element remainder
         */
        template <typename U = T>
        typename std::enable_if<bml_is_math_integral<U>::value, Matrix<T>>::type
        operator%(const T& scalar) const;

        /// Deleted when T is not in the allowed integral set.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix<T>>
        operator%(const T&) const = delete;



        // ---------- (1) Compound assignment: Matrix ⊕= Matrix ----------

        /**
         * @brief In-place element-wise addition with another matrix.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * After:
         *     this->operator+=(other);
         * each element satisfies:
         *     this(r,c) = this(r,c) + other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix& RHS matrix (same shape)
         * @return Matrix& reference to *this
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator+=(const Matrix& other);

        /// Deleted when T is not arithmetic.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator+=(const Matrix&) = delete;


        /**
         * @brief In-place element-wise subtraction with another matrix.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * this(r,c) -= other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix& RHS matrix
         * @return Matrix& reference to *this
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator-=(const Matrix& other);

        /// Deleted when T is not arithmetic.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator-=(const Matrix&) = delete;


        /**
         * @brief In-place element-wise multiplication with another matrix.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * this(r,c) *= other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix& RHS matrix
         * @return Matrix& reference to *this
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator*=(const Matrix& other);

        /// Deleted when T is not arithmetic.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator*=(const Matrix&) = delete;


        /**
         * @brief In-place element-wise division with another matrix.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * this(r,c) /= other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix& RHS matrix
         * @return Matrix& reference to *this
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator/=(const Matrix& other);

        /// Deleted when T is not arithmetic.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator/=(const Matrix&) = delete;


        /**
         * @brief In-place element-wise modulus with another matrix.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * this(r,c) %= other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix& RHS matrix
         * @return Matrix& reference to *this
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator%=(const Matrix& other);

        /// Deleted when T is not in the allowed integral set.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator%=(const Matrix&) = delete;



        // ---------- (1) Compound assignment: Matrix ⊕= scalar ----------

        /**
         * @brief In-place element-wise addition with a scalar.
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * this(r,c) += s
         *
         * @param s const T& scalar RHS
         * @return Matrix& reference to *this
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator+=(const T& s);

        /// Deleted when T is not arithmetic.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator+=(const T&) = delete;


        /**
         * @brief In-place element-wise subtraction with a scalar.
         *
         * this(r,c) -= s
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * @param s const T& scalar RHS
         * @return Matrix& reference to *this
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator-=(const T& s);

        /// Deleted when T is not arithmetic.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator-=(const T&) = delete;


        /**
         * @brief In-place element-wise multiplication with a scalar.
         *
         * this(r,c) *= s
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * @param s const T& scalar RHS
         * @return Matrix& reference to *this
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator*=(const T& s);

        /// Deleted when T is not arithmetic.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator*=(const T&) = delete;


        /**
         * @brief In-place element-wise division with a scalar.
         *
         * this(r,c) /= s
         *
         * Enabled only if T satisfies bml_is_math_arithmetic<T>.
         *
         * @param s const T& scalar RHS
         * @return Matrix& reference to *this
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_arithmetic<U>::value, Matrix&>
        operator/=(const T& s);

        /// Deleted when T is not arithmetic.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_arithmetic<U>::value, Matrix&>
        operator/=(const T&) = delete;


        /**
         * @brief In-place element-wise modulus with a scalar.
         *
         * this(r,c) %= s
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * @param s const T& scalar RHS
         * @return Matrix& reference to *this
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator%=(const T& s);

        /// Deleted when T is not in the allowed integral set.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator%=(const T&) = delete;



        // ---------- Bitwise element-wise (Matrix ⊗ Matrix) ----------

        /**
         * @brief Element-wise bitwise AND between two matrices.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * result(r,c) = this(r,c) & other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix& RHS matrix (same shape)
         * @return Matrix  per-element bitwise AND
         *
         * @throws if matrix dimensions do not match
         *
         * @see operator|()
         * @see operator^()
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator&(const Matrix& other) const;

        /// Deleted when T is not integral by our rules.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator&(const Matrix&) const = delete;


        /**
         * @brief Element-wise bitwise OR between two matrices.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * result(r,c) = this(r,c) | other(r,c)
         *
         * Throws if dimensions differ.
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator|(const Matrix& other) const;

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator|(const Matrix&) const = delete;


        /**
         * @brief Element-wise bitwise XOR between two matrices.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * result(r,c) = this(r,c) ^ other(r,c)
         *
         * Throws if dimensions differ.
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator^(const Matrix& other) const;

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator^(const Matrix&) const = delete;



        // ---------- Bitwise element-wise (Matrix ⊗ scalar) ----------

        /**
         * @brief Element-wise bitwise AND with a scalar.
         *
         * result(r,c) = this(r,c) & s
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * @param s const T& RHS scalar
         * @return Matrix per-element bitwise AND
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator&(const T& s) const;

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator&(const T&) const = delete;


        /**
         * @brief Element-wise bitwise OR with a scalar.
         *
         * result(r,c) = this(r,c) | s
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator|(const T& s) const;

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator|(const T&) const = delete;


        /**
         * @brief Element-wise bitwise XOR with a scalar.
         *
         * result(r,c) = this(r,c) ^ s
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator^(const T& s) const;

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator^(const T&) const = delete;



        // ---------- Compound bitwise ----------

        /**
         * @brief In-place element-wise bitwise AND with another matrix.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * this(r,c) &= other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix& RHS matrix (same shape)
         * @return Matrix& reference to *this
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator&=(const Matrix& other);

        /// Deleted when T is not integral.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator&=(const Matrix&) = delete;


        /**
         * @brief In-place element-wise bitwise OR with another matrix.
         *
         * this(r,c) |= other(r,c)
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         */
        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator|=(const Matrix& other);

        /// Deleted when T is not integral.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator|=(const Matrix&) = delete;


        /**
         * @brief In-place element-wise bitwise XOR with another matrix.
         *
         * this(r,c) ^= other(r,c)
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         */
        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator^=(const Matrix& other);

        /// Deleted when T is not integral.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator^=(const Matrix&) = delete;


        /**
         * @brief In-place element-wise bitwise AND with a scalar.
         *
         * this(r,c) &= s
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         */
        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator&=(const T& s);

        /// Deleted when T is not integral.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator&=(const T&) = delete;


        /**
         * @brief In-place element-wise bitwise OR with a scalar.
         *
         * this(r,c) |= s
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         */
        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator|=(const T& s);

        /// Deleted when T is not integral.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator|=(const T&) = delete;


        /**
         * @brief In-place element-wise bitwise XOR with a scalar.
         *
         * this(r,c) ^= s
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         */
        template <typename U = T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator^=(const T& s);

        /// Deleted when T is not integral.
        template <typename U = T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator^=(const T&) = delete;



        // ---------- Unary bitwise NOT ----------

        /**
         * @brief Element-wise bitwise NOT.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * result(r,c) = ~this(r,c)
         *
         * @return Matrix  bitwise NOT of each element
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator~() const;

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator~() const = delete;



        // ---------- Shifts (integrals only) ----------

        /**
         * @brief Element-wise left shift.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * result(r,c) = this(r,c) << k
         *
         * @param k int shift amount (number of bits)
         * @return Matrix per-element left-shifted result
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator<<(int k) const;

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator<<(int) const = delete;


        /**
         * @brief Element-wise right shift.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * result(r,c) = this(r,c) >> k
         *
         * @param k int shift amount (number of bits)
         * @return Matrix per-element right-shifted result
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix>
        operator>>(int k) const;

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix>
        operator>>(int) const = delete;


        /**
         * @brief In-place element-wise left shift.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * this(r,c) <<= k
         *
         * @param k int shift amount (number of bits)
         * @return Matrix& reference to *this
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator<<=(int k);

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator<<=(int) = delete;


        /**
         * @brief In-place element-wise right shift.
         *
         * Enabled only if T satisfies bml_is_math_integral<T>.
         *
         * this(r,c) >>= k
         *
         * @param k int shift amount (number of bits)
         * @return Matrix& reference to *this
         */
        template <typename U=T>
        std::enable_if_t<bml_is_math_integral<U>::value, Matrix&>
        operator>>=(int k);

        /// Deleted when T is not integral.
        template <typename U=T>
        std::enable_if_t<!bml_is_math_integral<U>::value, Matrix&>
        operator>>=(int) = delete;



        // ---------- Bool logical (only for T=bool) ----------

        /**
         * @brief Element-wise logical AND of two boolean matrices.
         *
         * Enabled only if T is bool.
         *
         * result(r,c) = this(r,c) && other(r,c)
         *
         * Throws if dimensions differ.
         *
         * @param other const Matrix& RHS bool matrix (same shape)
         * @return Matrix boolean matrix of per-element &&
         *
         * @throws if matrix dimensions do not match
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_and(const Matrix& other) const;

        /// Deleted when T is not bool.
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_and(const Matrix&) const = delete;


        /**
         * @brief Element-wise logical OR of two boolean matrices.
         *
         * Enabled only if T is bool.
         *
         * result(r,c) = this(r,c) || other(r,c)
         *
         * Throws if dimensions differ.
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_or(const Matrix& other) const;

        /// Deleted when T is not bool.
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_or(const Matrix&) const = delete;


        /**
         * @brief Element-wise logical XOR of two boolean matrices.
         *
         * Enabled only if T is bool.
         *
         * result(r,c) = (this(r,c) != other(r,c))
         *
         * Throws if dimensions differ.
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_xor(const Matrix& other) const;

        /// Deleted when T is not bool.
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_xor(const Matrix&) const = delete;


        /**
         * @brief Element-wise logical NOT of a boolean matrix.
         *
         * Enabled only if T is bool.
         *
         * result(r,c) = !this(r,c)
         *
         * @return Matrix boolean matrix of per-element !
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_not() const;

        /// Deleted when T is not bool.
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_not() const = delete;


        /**
         * @brief Element-wise logical AND of a boolean matrix and a bool scalar.
         *
         * Enabled only if T is bool.
         *
         * result(r,c) = this(r,c) && s
         *
         * @param s bool RHS scalar
         * @return Matrix boolean matrix of per-element &&
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_and(bool s) const;

        /// Deleted when T is not bool.
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_and(bool) const = delete;


        /**
         * @brief Element-wise logical OR of a boolean matrix and a bool scalar.
         *
         * Enabled only if T is bool.
         *
         * result(r,c) = this(r,c) || s
         *
         * @param s bool RHS scalar
         * @return Matrix boolean matrix of per-element ||
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_or(bool s) const;

        /// Deleted when T is not bool.
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_or(bool) const = delete;


        /**
         * @brief Element-wise logical XOR of a boolean matrix and a bool scalar.
         *
         * Enabled only if T is bool.
         *
         * result(r,c) = (this(r,c) != s)
         *
         * @param s bool RHS scalar
         * @return Matrix boolean matrix of per-element XOR
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, Matrix>
        logical_xor(bool s) const;

        /// Deleted when T is not bool.
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, Matrix>
        logical_xor(bool) const = delete;


        /**
         * @brief Count how many elements are true in a boolean matrix.
         *
         * Enabled only if T is bool.
         *
         * Iterates over all elements and returns how many are logically true.
         * This is mainly useful when treating a Matrix<bool> as a mask.
         *
         * The function is marked noexcept and will not throw.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return std::size_t  number of elements equal to true
         *
         * @see logical_and()
         * @see logical_or()
         * @see logical_xor()
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, std::size_t>
        count_true() const noexcept;

        /**
         * @brief Deleted for non-bool matrices.
         *
         * If T is not bool, this overload is selected and deleted,
         * causing a compile-time error. The idea is that "count how many
         * cells are true" only makes sense for Matrix<bool>.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         */
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, std::size_t>
        count_true() const noexcept = delete;


        /**
         * @brief Check if any element in the boolean matrix is true.
         *
         * Enabled only if T is bool.
         *
         * Returns true if at least one cell in the matrix evaluates to true.
         * Returns false if all cells are false or if the matrix is empty.
         *
         * This is noexcept; it does not throw.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return bool  true if any element is true, otherwise false
         *
         * @see none()
         * @see count_true()
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, bool>
        any() const noexcept;

        /**
         * @brief Deleted for non-bool matrices.
         *
         * If T is not bool, this overload is selected and deleted,
         * causing a compile-time error. "Does any cell evaluate true?"
         * is only defined for Matrix<bool>.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         */
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, bool>
        any() const noexcept = delete;


        /**
         * @brief Check if no element in the boolean matrix is true.
         *
         * Enabled only if T is bool.
         *
         * Returns true if all cells are false, or if the matrix is empty.
         * Returns false if at least one cell is true.
         *
         * This is logically equivalent to `!any()`, but may be implemented
         * without calling any().
         *
         * This is noexcept; it does not throw.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         * @return bool  true if there are zero true elements, otherwise false
         *
         * @see any()
         * @see count_true()
         */
        template <typename U = T>
        std::enable_if_t<bml_is_bool<U>::value, bool>
        none() const noexcept;

        /**
         * @brief Deleted for non-bool matrices.
         *
         * If T is not bool, this overload is selected and deleted,
         * causing a compile-time error. "Are there no true cells?"
         * is only defined for Matrix<bool>.
         *
         * @tparam U (do not pass explicitly) internal SFINAE hook, defaults to T
         */
        template <typename U = T>
        std::enable_if_t<!bml_is_bool<U>::value, bool>
        none() const noexcept = delete;

    };

        // ======================= Comparisons =======================

    /**
     * @brief Compare two matrices for equality.
     *
     * Returns true if and only if:
     *   1. `lhs` and `rhs` have the same dimensions (same number of rows and columns), and
     *   2. every corresponding element compares equal with `==`.
     *
     * Works for all T, including pointer types, std::string, etc.
     *
     * @tparam T element type stored in the matrices
     * @param lhs const Matrix<T>& left-hand operand
     * @param rhs const Matrix<T>& right-hand operand
     * @return bool true if matrices are equal in shape and contents, otherwise false
     */
    template <typename T>
    bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /**
     * @brief Inequality comparison for matrices.
     *
     * Logical negation of operator==().
     *
     * @tparam T element type stored in the matrices
     * @param lhs const Matrix<T>& left-hand operand
     * @param rhs const Matrix<T>& right-hand operand
     * @return bool true if matrices differ in shape or in at least one element
     *
     * @see operator==()
     */
    template <typename T>
    bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs);


    /**
     * @brief Lexicographic "less-than" comparison between two matrices.
     *
     * Enabled only if T is NOT a pointer type. For pointer element types
     * (e.g. void*, int*), this operator is SFINAE-disabled in the declaration,
     * so code like `Matrix<int*>::operator<` will fail at compile time.
     *
     * Ordering is defined in three steps:
     *
     *   1. Compare number of rows. A matrix with fewer rows is considered "less".
     *   2. If the row counts are equal, compare number of columns. A matrix with
     *      fewer columns is considered "less".
     *   3. If both shapes match exactly, compare elements in row-major order:
     *      (0,0), (0,1), … (0,n-1), (1,0), … etc. The first position `(i,j)`
     *      where `lhs(i,j) != rhs(i,j)` decides the ordering via `lhs(i,j) < rhs(i,j)`.
     *
     * If all elements compare equal, then `lhs < rhs` is false.
     *
     * @tparam T element type (must not be a pointer type)
     * @param lhs const Matrix<T>& left-hand operand
     * @param rhs const Matrix<T>& right-hand operand
     * @return bool true if lhs is considered lexicographically less than rhs
     *
     * @see operator>()
     * @see operator<=()
     * @see operator>=()
     */
    template <typename T,
              typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
    bool operator<(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /**
     * @brief Lexicographic "greater-than" comparison between two matrices.
     *
     * Enabled only if T is NOT a pointer type.
     *
     * Defined as `(rhs < lhs)`.
     *
     * @tparam T element type (must not be a pointer type)
     * @param lhs const Matrix<T>& left-hand operand
     * @param rhs const Matrix<T>& right-hand operand
     * @return bool true if lhs is considered lexicographically greater than rhs
     *
     * @see operator<()
     */
    template <typename T,
              typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
    bool operator>(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /**
     * @brief Lexicographic "less-than-or-equal" comparison between two matrices.
     *
     * Enabled only if T is NOT a pointer type.
     *
     * Defined as `!(rhs < lhs)`.
     *
     * @tparam T element type (must not be a pointer type)
     * @param lhs const Matrix<T>& left-hand operand
     * @param rhs const Matrix<T>& right-hand operand
     * @return bool true if lhs is not lexicographically greater than rhs
     *
     * @see operator<()
     * @see operator>()
     */
    template <typename T,
              typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
    bool operator<=(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /**
     * @brief Lexicographic "greater-than-or-equal" comparison between two matrices.
     *
     * Enabled only if T is NOT a pointer type.
     *
     * Defined as `!(lhs < rhs)`.
     *
     * @tparam T element type (must not be a pointer type)
     * @param lhs const Matrix<T>& left-hand operand
     * @param rhs const Matrix<T>& right-hand operand
     * @return bool true if lhs is not lexicographically less than rhs
     *
     * @see operator<()
     * @see operator>()
     */
    template <typename T,
              typename = typename std::enable_if<!std::is_pointer<T>::value>::type>
    bool operator>=(const Matrix<T>& lhs, const Matrix<T>& rhs);


    /**
     * @brief Stream insertion operator for Matrix.
     *
     * Writes a human-readable representation of the matrix to an output stream,
     * primarily for debugging / logging / diagnostic use.
     *
     * The exact formatting is the same as Matrix::toString(), and is not
     * guaranteed to be stable between library versions.
     *
     * Example:
     * @code
     * Matrix<int> m(2,2);
     * m.fill(7);
     * std::cout << "m =\n" << m << std::endl;
     * @endcode
     *
     * @tparam T element type
     * @param os std::ostream&  destination stream
     * @param mat const Matrix<T>& matrix to print
     * @return std::ostream& reference to @p os, to allow chaining
     *
     * @see Matrix::toString()
     */
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Matrix<T>& mat);


} // namespace bml

#endif // BML_MATRIX_HPP
