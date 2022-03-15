#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>

namespace player {
    class EventTrigger {
    public:
        EventTrigger() = default;
        ~EventTrigger();

        void load(const std::string &event_name, const std::function<void()> &callback);
        void unload(const std::string &event_name);
        bool trigger(const std::string &event_name);

    private:
        std::unordered_map<size_t, std::function<void()>> m_events;
    };
}
