#pragma once
#include <algorithm>

#include "../../vendor/proton/shared/common.h"

namespace utils {
    namespace math {
        template <typename T>
        T distance_xy(T x1, T y1, T x2, T y2) {
            return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
        }

        template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>, bool>>
        T distance_xy(CL_Vec2f v1, CL_Vec2f v2) {
            return distance_xy<T>(v1.x, v1.y, v2.x, v2.y);
        }

        template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, bool>>
        T distance_xy(CL_Vec2i v1, CL_Vec2i v2) {
            return static_cast<T>(distance_xy<float>(static_cast<float>(v1.x), static_cast<float>(v1.y), static_cast<float>(v2.x), static_cast<float>(v2.y)));
        }
    }
}