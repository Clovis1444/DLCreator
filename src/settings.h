#pragma once

#include <qstring.h>

namespace Settings {
inline static const QString kProgramName{"DLCreator"};

inline static const QString kCellLayerResourcesFilePath{
    "/home/clovis/dev/DLCreator/resources/cellLayer.json"};
inline static const QString kCellLayerResourcesDirPath{
    "/home/clovis/dev/DLCreator/resources/"};

inline static const QString kCellResourcesFilePath{
    "/home/clovis/dev/DLCreator/resources/cell.json"};
inline static const QString kCellResourcesDirPath{
    "/home/clovis/dev/DLCreator/resources/"};

namespace JSON {
namespace Cell {
inline static const QString kBackgroundFileKey{
    "default_cell_background_file_name"};
inline static const QString kFrameFileKey{"default_cell_frame_file_name"};
inline static const QString kActiveFrameFileKey{"active_cell_frame_file_name"};
}  // namespace Cell
namespace CellLayer {
inline static const QString kNameKey{"name"};
inline static const QString kFileKey{"file_name"};
}  // namespace CellLayer
}  // namespace JSON
}  // namespace Settings
