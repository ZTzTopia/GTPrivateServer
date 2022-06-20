#pragma once
#include <string_view>

namespace utils {
    namespace hash {
        constexpr std::size_t fnv1a(const std::string_view& data) {
            // Fowler/Noll/Vo 1a variant.
            std::size_t prime;
            std::size_t offset_basis;

            if constexpr (sizeof(std::size_t) == 8) {
                // 64-bit
                prime = 1099511628211ULL;
                offset_basis = 14695981039346656037ULL;
            }
            else {
                // 32-bit
                prime = 16777619U;
                offset_basis = 2166136261U;
            }

            std::size_t hash{ offset_basis };
            for (auto& c : data) {
                hash ^= c;
                hash *= prime;
            }

            return hash;
        }

        constexpr uint32_t proton(const char* data, std::size_t length = 0) {
            uint32_t hash = 0x55555555;
            if (data) {
                if (length > 0) {
                    while (length--)
                        hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const uint8_t*>(data++);
                }
                else {
                    while (*data)
                        hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const uint8_t*>(data++);
                }
            }

            return hash;
        }

        constexpr std::size_t operator "" _fh(const char* str, std::size_t len) {
            return fnv1a(std::string_view{ str, len });
        }

        constexpr uint32_t operator "" _ph(const char* str, std::size_t len) {
            return proton(str, static_cast<uint32_t>(len));
        }
    }
}
