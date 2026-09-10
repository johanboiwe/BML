/**
* @file json.hpp
 * @brief JSON support for BML.
 *
 * Provides the `bml::Json` type alias for the
 * [nlohmann/json](https://github.com/nlohmann/json) library.
 *
 * `Json` can be used as the element type of a BML matrix:
 *
 * @code
 * #include <BML/json.hpp>
 *
 * bml::Matrix<bml::Json> matrix(2, 2);
 *
 * matrix[0][0] = bml::Json{{"name", "Alice"}, {"age", 30}};
 * matrix[0][1] = bml::Json::array({1, 2, 3});
 * @endcode
 *
 * JSON matrices may contain heterogeneous JSON values. Each matrix element
 * may independently contain a null, boolean, number, string, array, or object.
 *
 * For serialisation, BML converts each JSON value to its textual
 * representation using `Json::dump()`. During deserialisation, the textual
 * representation is reconstructed using `Json::parse()`.
 *
 * @see bml::Json
 */

#ifndef BML_JSON_HPP
#define BML_JSON_HPP

#include <nlohmann/json.hpp>

/**
 * @namespace bml
 * @brief BML public API namespace.
 */
namespace bml
{
    /**
     * @brief JSON value type used by BML.
     *
     * `Json` is an alias for `nlohmann::json`.
     *
     * It can be used as the element type of `bml::Matrix`, allowing each
     * matrix cell to contain an independent JSON value.
     *
     * @code
     * bml::Json object = {
     *     {"name", "Alice"},
     *     {"age", 30}
     * };
     * @endcode
     *
     * JSON values do not participate in BML arithmetic operations or
     * ordering comparisons.
     *
     * Equality and inequality comparisons are supported.
     *
     * @note JSON serialisation uses `nlohmann::json::dump()` and
     * deserialisation uses `nlohmann::json::parse()`.
     */
    using Json = nlohmann::json;
}

#endif // BML_JSON_HPP