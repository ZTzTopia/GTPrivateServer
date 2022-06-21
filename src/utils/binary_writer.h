#pragma once
#include <cstdint>
#include <memory>
#include <string>

class BinaryWriter {
public:
    explicit BinaryWriter(void* data) : m_data(static_cast<uint8_t*>(data)), m_pos(0) {}
    ~BinaryWriter() = default;
    
    template <typename T>
    void write(T data)
    {
        std::memcpy(m_data + m_pos, &data, sizeof(T));
        m_pos += sizeof(T);
    }

    void write_string(const std::string& string)
    {
        write_u16(string.length());
        std::memcpy(m_data + m_pos, string.c_str(), string.length());
        m_pos += string.length();
    }

    void write_bytes(const void* data, std::size_t size)
    {
        std::memcpy(m_data + m_pos, data, size);
        m_pos += size;
    }

    void write_u8(uint8_t a) { write(a); }
    void write_u16(uint16_t a) { write(a); }
    void write_u32(uint32_t a) { write(a); }
    void write_u64(uint64_t a) { write(a); }
    void write_i8(int8_t a) { write(a); }
    void write_i16(int16_t a) { write(a); }
    void write_i32(int32_t a) { write(a); }
    void write_i64(int64_t a) { write(a); }

    void write_bool(bool a) { write(a); }
    void write_float(float a) { write(a); }
    void write_double(double a) { write(a); }

    void skip(std::size_t len) { m_pos += len; }
    void skip_string(const std::string& string) { m_pos += string.length(); }
    void back(std::size_t len) { m_pos -= len; }

    [[nodiscard]] std::size_t position() const { return m_pos; }

private:
    uint8_t* m_data;
    std::size_t m_pos;
};
