#pragma once
#include <cstdint>

namespace FAC
{
    // TO DO: Error handling
    inline std::vector<unsigned char> int32ToBytes(uint32_t value) {
        std::vector<unsigned char> bytes(4);
        bytes[0] = static_cast<unsigned char>((value >> 24) & 0xFF); 
        bytes[1] = static_cast<unsigned char>((value >> 16) & 0xFF);
        bytes[2] = static_cast<unsigned char>((value >> 8) & 0xFF);
        bytes[3] = static_cast<unsigned char>(value & 0xFF);         
        return bytes;
    }

    inline uint32_t bytesToInt32(const std::vector<unsigned char>& bytes) {

        return (static_cast<uint32_t>(bytes[0]) << 24) |
            (static_cast<uint32_t>(bytes[1]) << 16) |
            (static_cast<uint32_t>(bytes[2]) << 8) |
            static_cast<uint32_t>(bytes[3]);
    }
    inline std::vector<unsigned char> int64ToBytes(uint64_t value) {
        std::vector<unsigned char> bytes(8);
        bytes[0] = static_cast<unsigned char>((value >> 56) & 0xFF); 
        bytes[1] = static_cast<unsigned char>((value >> 48) & 0xFF);
        bytes[2] = static_cast<unsigned char>((value >> 40) & 0xFF);
        bytes[3] = static_cast<unsigned char>((value >> 32) & 0xFF);
        bytes[4] = static_cast<unsigned char>((value >> 24) & 0xFF);
        bytes[5] = static_cast<unsigned char>((value >> 16) & 0xFF);
        bytes[6] = static_cast<unsigned char>((value >> 8) & 0xFF);
        bytes[7] = static_cast<unsigned char>(value & 0xFF);        
        return bytes;
    }
    inline uint64_t bytesToInt64(const std::vector<unsigned char>& bytes) {
        return (static_cast<uint64_t>(bytes[0]) << 56) |
            (static_cast<uint64_t>(bytes[1]) << 48) |
            (static_cast<uint64_t>(bytes[2]) << 40) |
            (static_cast<uint64_t>(bytes[3]) << 32) |
            (static_cast<uint64_t>(bytes[4]) << 24) |
            (static_cast<uint64_t>(bytes[5]) << 16) |
            (static_cast<uint64_t>(bytes[6]) << 8) |
            static_cast<uint64_t>(bytes[7]);
    }
}