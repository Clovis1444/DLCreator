#pragma once

#include <qcontainerfwd.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qvarlengtharray.h>

#include <utility>

#include "src/mapSaver/mapSaver.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cellItem/cellItem.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cellItem/cellLayer.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/gridManager.h"

namespace MapSaverJsons {

class MapCellObj : public QObject {
    Q_OBJECT

   public:
    explicit MapCellObj(const CellItem::CellInfo& ci) {
        for (int i{0}; i < CellLayer::Type::kEnumLength; ++i) {
            CellLayer::Type type{static_cast<CellLayer::Type>(i)};

            cell_.insert(CellLayer::jsonName(type), ci[type]);
            cell_.insert(MapSaver::kCellSelectedKey, ci.selected);
        }
    }
    explicit MapCellObj(QJsonObject json_obj) : cell_{std::move(json_obj)} {}
    MapCellObj(const MapCellObj& other) : cell_{other.cell_} {};

    QString operator[](CellLayer::Type type) const {
        if (type == CellLayer::Type::kEnumLength) return "";

        return cell_[CellLayer::jsonName(type)].toString();
    }

    CellItem::CellInfo cellInfo() const {
        CellItem::CellInfo ci;

        for (int i{0}; i < CellLayer::Type::kEnumLength; ++i) {
            CellLayer::Type l_type{static_cast<CellLayer::Type>(i)};
            ci[l_type] = (*this)[l_type];
        }

        ci.selected = selected();

        return ci;
    }

    bool selected() const { return cell_[MapSaver::kCellSelectedKey].toBool(); }

    QJsonObject json() const { return cell_; }

    bool isValid() const {
        // Check if all layers are presented
        for (int i{0}; i < CellLayer::Type::kEnumLength - 1; ++i) {
            if (!cell_.contains(
                    CellLayer::jsonName(static_cast<CellLayer::Type>(i)))) {
                return false;
            }
        }

        // Check if selected is presented
        return cell_.contains(MapSaver::kCellSelectedKey);
    }

   protected:
    QJsonObject cell_;
};

class MapCellsObj : public QObject {
    Q_OBJECT

   public:
    explicit MapCellsObj(const GridManager* gm) {
        for (const CellItem* i : gm->cellList()) {
            CellItem::CellInfo ci{i->info()};
            QPoint cpos{i->gridPos()};

            MapCellObj cell_obj{ci};

            QString cell_name{QString{"%1_%2"}.arg(cpos.x()).arg(cpos.y())};
            // Insert current cell to cell object
            cells_.insert(cell_name, cell_obj.json());
        };
    }
    explicit MapCellsObj(const QJsonObject& json_obj) {
        for (const auto& i : json_obj.keys()) {
            QJsonObject cell_json{json_obj[i].toObject()};

            MapCellObj cell_obj{cell_json};

            cells_.insert(i, cell_obj.json());
        }
    }
    MapCellsObj(const MapCellsObj& other) : cells_{other.cells_} {};

    MapCellObj operator[](const QString& cell_name) const {
        return MapCellObj{cells_[cell_name].toObject()};
    }

    QJsonObject json() const { return cells_; }

    // By default returns true if contains at least 1 object
    // If strict_validation - also checks if every containing object is valid
    bool isValid(bool strict_validation = false) const {
        if (cells_.count() == 0) {
            return false;
        }

        if (!strict_validation) {
            return true;
        }

        // NOLINTNEXTLINE
        for (const auto& i : cells_) {
            MapCellObj cell_obj{i.toObject()};

            if (!cell_obj.isValid()) {
                return false;
            }
        }

        return true;
    }

   protected:
    QJsonObject cells_;
};

class MapSizeArr : public QObject {
    Q_OBJECT
   public:
    explicit MapSizeArr(const QPair<int, int>& map_size)
        : map_size_{map_size.first, map_size.second} {}
    explicit MapSizeArr(const QJsonArray& json_arr)
        : map_size_{json_arr[0], json_arr[1]} {}
    MapSizeArr(const MapSizeArr& other) : map_size_{other.map_size_} {};

    QPair<int, int> size() const {
        return {map_size_[0].toInt(), map_size_[1].toInt()};
    }
    int x() const { return map_size_[0].toInt(); }
    int y() const { return map_size_[1].toInt(); }

    QJsonArray json() const { return map_size_; }

    bool isValid() const {
        return map_size_.count() == 2 && map_size_[0].toInt(-1) != -1 &&
               map_size_[1].toInt(-1) != -1;
    }

   protected:
    QJsonArray map_size_;
};

// {
//      "comment": "",
//      "dlmap": {
//          "map_size": [1, 1],
//          "Cells": {...}
//      }
// }
class MapJson : public QObject {
    Q_OBJECT

   public:
    MapJson(const MapCellsObj& cells, const MapSizeArr& size_arr)
        : cells_{cells}, size_arr_{size_arr} {
        constructorBody();
    }
    explicit MapJson(const GridManager* gm)
        : cells_{gm}, size_arr_{gm->gridSize()} {
        constructorBody();
    }
    explicit MapJson(const QJsonDocument& json)
        : cells_{json[MapSaver::kRootObjKey][MapSaver::kRootCellsKey]
                     .toObject()},
          size_arr_{json[MapSaver::kRootObjKey][MapSaver::kRootMapSizeKey]
                        .toArray()} {
        constructorBody();
    }

    QString comment() const {
        return json_root_[MapSaver::kFileCommentKey].toString();
    }
    MapCellsObj cells() const { return cells_; }
    MapSizeArr size_arr() const { return size_arr_; }
    QJsonObject dlmap_json() const {
        return json_root_[MapSaver::kRootObjKey].toObject();
    }

    QPair<int, int> size() const { return size_arr_.size(); }
    int rows() const { return size_arr_.x(); }
    int cols() const { return size_arr_.y(); }

    QJsonObject json() const { return json_root_; }

    bool isValid(bool strict_validation = false) const {
        return !json_root_[MapSaver::kFileCommentKey].isUndefined() &&
               !json_root_[MapSaver::kRootObjKey].isUndefined() &&
               cells_.isValid(strict_validation) && size_arr_.isValid();
    }

   protected:
    void constructorBody() {
        QJsonObject dlmap_obj{{MapSaver::kRootMapSizeKey, size_arr_.json()},
                              {MapSaver::kRootCellsKey, cells_.json()}};

        json_root_ = QJsonObject{{MapSaver::kFileCommentKey,
                                  "This is a dungeon layout map save file"},
                                 {MapSaver::kRootObjKey, dlmap_obj}};
    }

    MapCellsObj cells_;
    MapSizeArr size_arr_;
    QJsonObject json_root_;
};
};  // namespace MapSaverJsons
