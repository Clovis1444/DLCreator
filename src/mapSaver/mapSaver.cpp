#include "mapSaver.h"

#include <qlogging.h>

#include "src/mapSaver/mapSaverJsons.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/gridManager.h"

QJsonDocument MapSaver::saveMapToFile(const GridManager* gm) {
    QJsonDocument json{MapSaverJsons::MapJson{gm}.json()};

    return json;
}

GridManager* MapSaver::loadMapFromFile(const QString& file_name,
                                       const QByteArray& file_content) {
    QJsonParseError err{};
    QJsonDocument json{QJsonDocument::fromJson(file_content, &err)};

    MapSaverJsons::MapJson map{json};

    // Return if failed to read map_size
    if (!map.size_arr().isValid()) {
        qWarning() << QString{"Failed to read %1: [%2][%3] is not valid"}
                          .arg(file_name)
                          .arg(kRootObjKey)
                          .arg(kRootMapSizeKey);

        return nullptr;
    }

    // Return if failed to read cells
    if (!map.cells().isValid()) {
        qWarning() << QString{"Failed to read %1: [%2][%3] is not valid"}
                          .arg(file_name)
                          .arg(kRootObjKey)
                          .arg(kRootCellsKey);

        return nullptr;
    }

    // Throw warning if not all cells are valid
    if (!map.cells().isValid(true)) {
        qInfo() << QString{"Warning while reading %1: not all cells in [%2][%3] are valid"}
                        .arg(file_name)
                        .arg(kRootObjKey)
                        .arg(kRootCellsKey);
    }

    GridManager* gm{new GridManager{nullptr, map.rows(), map.cols()}};

    // Load cells
    for (auto* i : gm->cellItemsMut()) {
        QPoint c_pos{i->gridPos()};

        QString cell_key{QString{"%1_%2"}.arg(c_pos.x()).arg(c_pos.y())};

        auto cell_obj{map.cells()[cell_key]};

        // Skip if cell was not found
        if (!cell_obj.isValid()) {
            qInfo() << QString{"Warning: cell %1 was not found"}.arg(cell_key);
            continue;
        }

        // Load cell info from json
        i->setLayer(cell_obj.cellInfo());
    }

    return gm;
}

// Generates absolute save file path with the file extension
// If add_number is set - adds number in parenthesis at the end of file name if
// the file already exists
QString MapSaver::getSaveFilePath(const QString& name, bool add_number) {
    QFile save_file{Settings::SavesDir() + name + Settings::kSaveFileExtension};

    // If file does not exists(or add_number is not set) - return default file
    // name
    if (!add_number || !save_file.exists()) {
        return save_file.fileName();
    }

    // If file already exists - return file name with number
    int count{1};
    while (true) {
        save_file.setFileName(Settings::SavesDir() + name + '(' +
                              QString::number(count) + ')' +
                              Settings::kSaveFileExtension);

        if (!save_file.exists()) {
            return save_file.fileName();
        }

        ++count;
    }
}
