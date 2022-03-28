#pragma once
#include <cstdint>

#include "../../vendor/proton/shared/common.h"

namespace world {
    struct Object {
        enum eObjectChangeType {
            OBJECT_CHANGE_TYPE_COLLECT = 0,
            OBJECT_CHANGE_TYPE_ADD = -1,
            OBJECT_CHANGE_TYPE_REMOVE = -2,
            OBJECT_CHANGE_TYPE_MODIFY = -3,
        };

        uint16_t object_id;
        uint8_t flags;
        uint8_t object_amount;
        CL_Vec2i pos;
    };
}
