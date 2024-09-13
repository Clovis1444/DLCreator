#include <qcontainerfwd.h>
#include <qjsondocument.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellLayer.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cellCollection.h"

class MapSave : public QObject {
    Q_OBJECT

   public:
    static QJsonDocument saveMapToFile(const CellCollection* cc) {
        // Obj containing all cells
        QJsonObject json_cells{};
        // Iterate cells
        for (const auto* i : cc->cellList()) {
            Cell::CellInfo ci{i->info()};
            QPoint cpos{i->pos()};

            QJsonObject cell_obj{
                {CellLayer::jsonName(CellLayer::Type::kBackground),
                 ci.background},
                {CellLayer::jsonName(CellLayer::Type::kLiquid), ci.liquid},
                {CellLayer::jsonName(CellLayer::Type::kGaz), ci.gaz},
                {kCellSelectedKey, ci.selected}};

            QString cell_name{QString{"%1_%2"}.arg(cpos.x()).arg(cpos.y())};
            // Insert current cell to cell object
            json_cells.insert(cell_name, cell_obj);
        };

        // map_size array
        QPair<int, int> map_size{cc->gridSize()};
        QJsonArray json_arr{map_size.first, map_size.second};
        // Root dlmap object: contains map size and all cells
        QJsonObject json_obj{{kRootMapSizeKey, json_arr},
                             {kRootCellsKey, json_cells}};

        // dlmap root object and comment
        QJsonObject json_obj_root{
            {kFileCommentKey, "This is a dungeon layout map save file"},
            {kRootObjKey, json_obj}};

        QJsonDocument json{json_obj_root};

        return json;
    }

    // TODO(clovis): implement return CellCollection*
    static void loadMapFromFile(const QString& file_name,
                                const QByteArray& file_content) {
        QJsonParseError err{};
        QJsonDocument map_json{QJsonDocument::fromJson(file_content, &err)};

        // Return if json read error
        if (err.error != QJsonParseError::NoError) {
            qDebug() << QString{"Failed to read %1 file: "}.arg(
                            Settings::kSaveFileExtension)
                     << err.errorString();
            return;
        }

        qDebug() << map_json["dlmap"]["Cells"]["10_6"];
    }

    inline static const QString kRootObjKey{
        Settings::kSaveFileExtension.sliced(1)};
    inline static const QString kFileCommentKey{"comment"};

    inline static const QString kRootMapSizeKey{"map_size"};
    inline static const QString kRootCellsKey{"Cells"};
    inline static const QString kCellSelectedKey{"Selected"};
    inline static const QString kCellPosKey{"Pos"};
};