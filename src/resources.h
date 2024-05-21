#pragma once

#include "widgets/cell/cell.h"
#include "widgets/cell/celllayer.h"

namespace Resources {
inline void init_resources() {
    loadCellLayersFromJson();
    Cell::loadResourcesFromJson();
}
}  // namespace Resources