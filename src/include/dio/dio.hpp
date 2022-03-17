#pragma once
#include <string>
#include <array>
#include <fstream>

namespace dio {
    class in {
    public:
        explicit in(const std::string &filename) : writer(filename, std::ofstream::in | std::ofstream::binary) {}
        ~in() { if (writer.is_open()) writer.close(); };

        // Is useless right?
        void write_header(uint32_t version) {
            writer.write(reinterpret_cast<const char *>(&version), sizeof(uint32_t));
        }

        void write(const char *data, long long size) {
            writer.write(data, size);
        }

        template <typename T>
        void write(T data) {
            write(reinterpret_cast<const char *>(&data), sizeof(T));
        }

        void write(const std::string &value) {
            write<unsigned short>(static_cast<unsigned short>(value.size()));
            for (auto &c : value) {
                write<unsigned char>(static_cast<unsigned char>(c) ^ 'a');
            }
        }

        void write(const char *value) {
            write(std::string{ value });
        }

        void write(char *value) {
            write<const char *>(value);
        }

        void close() {
            writer.close();
        }

    private:
        std::ofstream writer;
    };

    class out {
    public:
        explicit out(const std::string &filename) : reader(filename, std::ofstream::out | std::ofstream::binary) {}
        ~out() { if (reader.is_open()) reader.close(); };

        // Is useless right?
        uint32_t read_header() {
            uint32_t version = 0;
            reader.read(reinterpret_cast<char *>(&version), sizeof(uint32_t));
            return version;
        }

        void read(char *data, long long size) {
            reader.read(data, size);
        }

        template <typename T>
        T read() {
            std::array<char, sizeof(T)> data{};
            read(data.data(), sizeof(T));
            return *reinterpret_cast<T*>(data.data());
        }

        std::string read() {
            auto length = read<unsigned short>();
            std::string string(length, '\0');
            read(string.data(), length);
            for (auto &c : string) {
                c ^= 'a';
            }
            return string;
        }

        void close() {
            reader.close();
        }

    private:
        std::ifstream reader;
    };
}