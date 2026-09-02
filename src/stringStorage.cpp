//
// Created by johan on 2025-10-14.
//

#include "../include/stringStorage.hpp"

#include <vector>

namespace bml
{
    StringStorage::StringStorage(const std::uint8_t* bytes)
    {
        const std::uint16_t len = static_cast<std::uint16_t>(bytes[0] | (bytes[1] << 8));
        data.assign(reinterpret_cast<const char*>(bytes + 2), len);
        length = len;
    }

    StringStorage::StringStorage(const std::string& string)
    {
        if (string.length() > 1<<16) length = (1<<16)-1;
        else length = string.length();
        data.resize(length);
        for (std::uint16_t i = 0; i < length; i++)
        {
            data[i] = string[i];
        }



    }

    std::string StringStorage::toString()
    {
        return data;
    }

    std::vector<std::uint8_t> StringStorage::toBytes() const
    {
        std::vector<uint8_t> result;
        result.reserve(static_cast<std::size_t>(2) + length);
        // Little-endian length field: low byte, then high byte
        result.push_back(static_cast<std::uint8_t>(length & 0xFF));
        result.push_back(static_cast<std::uint8_t>((length >> 8) & 0xFF));

        // Append payload (no NUL terminator)
        result.insert(result.end(),
                   reinterpret_cast<const std::uint8_t*>(data.data()),
                   reinterpret_cast<const std::uint8_t*>(data.data()) + length);

        return result;




    }

    std::uint16_t StringStorage::len() const
    {
        return length;
    }
} // bml