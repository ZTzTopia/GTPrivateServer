#include "eventmanager.h"

namespace player {
    EventManager::~EventManager() {
        m_events.clear();
    }

    /*void EventManager::load(const std::string &event_name, const std::function<void(const std::string &)> &callback) {
        if (event_name.empty()) {
            return;
        }

        if (callback == nullptr) {
            return;
        }

        m_events.insert_or_assign(std::hash<std::string>{}(event_name), callback);
    }

    void EventManager::unload(const std::string &event_name) {
        if (event_name.empty()) {
            return;
        }

        m_events.erase(std::hash<std::string>{}(event_name));
    }

    bool EventManager::execute(const std::string &event_name, const std::string &string) {
        if (event_name.empty()) {
            return false;
        }

        auto it = m_events.find(std::hash<std::string>{}(event_name));
        if (it != m_events.end()) {
            it->second(string);
            return true;
        }

        return false;
    }*/
}