#include "eventtrigger.h"

namespace player {
    EventTrigger::~EventTrigger() {
        m_events.clear();
    }

    void EventTrigger::load(const std::string &event_name, const std::function<void()> &callback) {
        if (event_name.empty()) {
            return;
        }

        if (callback == nullptr) {
            return;
        }

        m_events.insert_or_assign(std::hash<std::string>{}(event_name), callback);
    }

    void EventTrigger::unload(const std::string &event_name) {
        if (event_name.empty()) {
            return;
        }

        m_events.erase(std::hash<std::string>{}(event_name));
    }

    bool EventTrigger::trigger(const std::string &event_name) {
        if (event_name.empty()) {
            return false;
        }

        auto it = m_events.find(std::hash<std::string>{}(event_name));
        if (it != m_events.end()) {
            it->second();
            return true;
        }

        return false;
    }
}