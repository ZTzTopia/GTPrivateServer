#pragma once
#include <string>
#include <vector>

class TextParse {
public:
    TextParse() = default;
    TextParse(const std::string &data) {
        m_data = string_tokenize(data, "\n");
        for (unsigned int i = 0; i < m_data.size(); i++) {
            string_replace(m_data[i], "\r", "");
        }
    };
    ~TextParse() = default;

    // Make it short and readable.
    static void string_replace(std::string &str, const std::string &from, const std::string &to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    // Make it short and readable.
    static std::vector<std::string> string_tokenize(const std::string &str, const std::string &delimiters) {
        std::vector<std::string> tokens;
        size_t start = 0, end;
        while ((end = str.find_first_of(delimiters, start)) != std::string::npos) {
            if (end != start) {
                tokens.push_back(str.substr(start, end - start));
            }

            start = end + 1;
        }

        if (start < str.length()) {
            tokens.push_back(str.substr(start));
        }

        return tokens;
    }

    std::string get(const std::string &key, int index, const std::string &token = "|") {
        if (m_data.empty()) {
            return "";
        }

        for (unsigned int i = 0; i < m_data.size(); i++) {
            if (m_data[i].empty()) {
                continue;
            }

            std::vector<std::string> data = string_tokenize(m_data[i], token);
            if (data[0] == key) {
                if (index < 0 && i + index > m_data.size()) {
                    return "";
                }

                // Found it.
                return data[index];
            }
        }

        return "";
    }

    template<typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
    T get(const std::string &key, int index, const std::string &token = "|") {
        return std::stoi(get(key, index, token));
    }

    template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    T get(const std::string &key, int index, const std::string &token = "|") {
        if (std::is_same_v<T, double>) {
            return std::stod(get(key, index, token));
        }
        else if (std::is_same_v<T, long double>) {
            return std::stold(get(key, index, token));
        }

        return std::stof(get(key, index, token));
    }

    void add(const std::string &key, const std::string &value, const std::string &token = "|") {
        std::string data = key + token + value;
        m_data.push_back(data);
    }

    template<typename T, typename std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = true>
    void add(const std::string &key, const T &value, const std::string &token = "|") {
        add(key, std::to_string(value), token);
    }

    void set(const std::string &key, const std::string &value, const std::string &token = "|") {
        if (m_data.empty()) {
            return;
        }

        for (unsigned int i = 0; i < m_data.size(); i++) {
            std::vector<std::string> data = string_tokenize(m_data[i], token);
            if (data[0] == key) {
                m_data[i] = data[0];
                m_data[i] += token;
                m_data[i] += value;
                break;
            }
        }
    }

    template<typename T, typename std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = true>
    void set(const std::string &key, const T &value, const std::string &token = "|") {
        set_(key, std::to_string(value), token);
    }

    void get_all_raw(std::string &data) {
        if (m_data.empty()) {
            return;
        }

        std::string string{};
        for (unsigned int i = 0; i < m_data.size(); i++) {
            string += m_data[i];
            if (!m_data[i + 1].empty()) {
                string += "\n";
            }
        }

        data = string;
    }

    size_t get_line_count() {
        return m_data.size();
    }

private:
    std::vector<std::string> m_data;
};