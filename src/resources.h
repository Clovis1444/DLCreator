#pragma once

#include "widgets/contentWidget/cell/cell.h"
#include "widgets/contentWidget/cell/cellLayer.h"

namespace Resources {
inline void init_resources() {
    loadCellLayersFromJson();
    Cell::loadResourcesFromJson();
}
}  // namespace Resources