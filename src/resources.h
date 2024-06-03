#pragma once

#include "widgets/tabWidget/stackedContentWidget/contentWidget/cell/cell.h"
#include "widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellLayer.h"

namespace Resources {
inline void init_resources() {
    loadCellLayersFromJson();
    Cell::loadResourcesFromJson();
}
}  // namespace Resources