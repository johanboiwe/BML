#ifndef BML_HPP
#define BML_HPP

/**
 * @file bml/bml.hpp
 * @brief Master include for the BML library.
 *
 * Including this header pulls in the main public interfaces of BML:
 *  - Platform requirements and compile-time checks
 *  - Matrix container and its views
 *  - Iterators
 *  - Special handling for bool storage
 *  - Version macros
 *  - StringStorage (length-prefixed string helper)
 *
 * Typical use:
 * @code
 * #include <bml/BML.hpp>
 *
 * int main() {
 *     bml::Matrix<int> m(3, 3);
 *     // ...
 * }
 * @endcode
 *
 * You can also include individual headers (e.g. <bml/matrix.hpp>) if you want
 * faster compile times, but <bml/BML.hpp> is the "just give me BML" option.
 */

#include "export.hpp"          // BML_API / BML_HIDDEN visibility macros
#include "platform.hpp"        // 64-bit platform check
#include "version.hpp"         // version macros

#include "traversal.hpp"       // bml::TraversalType enum (traversal order)

#include "matrix.hpp"          // Matrix<T>
#include "iterator.hpp"        // Iterators / iteration helpers
#include "rowView.hpp"         // RowView / ColView proxies
#include "boolRef.hpp"         // BoolRef proxy for Matrix<bool>
#include "stringStorage.hpp"   // StringStorage helper for Matrix<std::string>

#endif // BML_HPP
