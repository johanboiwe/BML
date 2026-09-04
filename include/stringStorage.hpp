//
// Created by johan on 2025-10-14.
//

#ifndef BML_STRINGSTORAGE_HPP
#define BML_STRINGSTORAGE_HPP
#include <cstdint>
#include <string>
#include <vector>
#include "export.hpp"
namespace bml
{

    /**
     * @brief A Pascal string inspired string storage format, using a 16-bit unsigned integer to store the length of the string
     */
    class StringStorage
    {
    private:

        /**
         * @brief A 16-bit unsigned integer holding the length of the string
         */
        std::uint16_t length;

        /**
         * @brief A standard c++-string storing the stirng's actual data
         */
        std::string data;

    public:

        /**
         * @brief StirngStorage constructor, construction from bytes
         * @param data A bytestream beginning with a 16-bit unsigned integer, followed by the actual string
         */
        explicit StringStorage(const std::uint8_t* data);


        /**
         * @brief StringStorage construction, construction the object from a standard c++ string
         * @param string A standard c++ string, to be copied and stored inside the StringStorage object.
         */
        explicit StringStorage(const std::string& string);


        /**
         * @brief Creates a c++-string copy of the storage string
         * @return string. A copy of StringStorage's content
         */
        std::string toString();

        /**
         * @brief Creates a buffer in the format that StringStorage(std::uint8_t* data) can interpret
         * @return A uint8 buffer;
         */
        [[nodiscard]] std::vector<std::uint8_t> toBytes() const;

        /**
         * @brief Returns the length of the string
         * @return uint16_t
         */
        [[nodiscard]] std::uint16_t len() const;




    };
} // bml

#endif //BML_STRINGSTORAGE_HPP