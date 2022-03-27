#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace cluster {
    class EventEmitter {
    public:
        EventEmitter() = default;
        ~EventEmitter() { off(); }

        template <typename Callback>
        void on(const std::string &name, Callback cb, bool once = false) {
            auto f = to_function(cb);
            auto fn = new decltype(f)(to_function(cb));
            m_events[name].emplace_back(static_cast<void *>(fn), once);
        }

        template <typename Callback>
        void once(const std::string& name, Callback cb) {
            on(name, cb, true);
        }

        void off() {
            std::for_each(m_events.begin(), m_events.end(), [](auto &pair) {
                auto& listeners = pair.second;
                std::for_each(listeners.begin(), listeners.end(), [](auto &listener) {
                    ::operator delete(std::get<0>(listener));
                });
            });

            m_events.clear();
        }

        void off(const std::string &name) {
            auto it = m_events.find(name);
            if (it != m_events.end()) {
                auto& listeners = it->second;
                std::for_each(listeners.begin(), listeners.end(), [](auto& listener) {
                    ::operator delete(std::get<0>(listener));
                });

                m_events.erase(it);
            }
        }

        template <typename ...Args>
        bool emit(const std::string& name, Args... args) {
            auto it = m_events.find(name);
            if (it != m_events.end()) {
                auto &listeners = it->second;
                for (int i = 0; i < listeners.size(); i++) {
                    auto &listener = listeners.at(i);
                    auto fn = static_cast<std::function<void(Args...)> *>(std::get<0>(listener));
                    (*fn)(args...);

                    if (std::get<1>(listener)) {
                        ::operator delete(std::get<0>(listener));
                        listeners.erase(listeners.begin() + i);
                        listeners.shrink_to_fit();
                    }
                }

                return true;
            }

            return false;
        }

    private:
        template <typename Callback>
        struct traits : public traits<decltype(&Callback::operator())> {};

        template <typename ClassType, typename R, typename... Args>
        struct traits<R(ClassType:: *)(Args...) const> {
            using fn = std::function<R (Args...)>;
        };

        template <typename Callback>
        typename traits<Callback>::fn to_function(Callback& cb) {
            return static_cast<typename traits<Callback>::fn>(cb);
        }

    private:
        std::unordered_map<std::string, std::vector<std::tuple<void *, bool>>> m_events;
    };
}
