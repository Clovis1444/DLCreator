#pragma once

#include <qapplication.h>
#include <qstring.h>

class Settings {
   public:
    inline static const QString kProgramName{"DLCreator"};

    inline static const QString kSaveFileExtension{".dlmap"};

    static QString CellLayerResourcesFilePath() {
        return QApplication::applicationDirPath() + kCellLayerResourcesFilePath;
    }
    static QString CellLayerResourcesDirPath() {
        return QApplication::applicationDirPath() + kCellLayerResourcesDirPath;
    }

    static QString CellResourcesFilePath() {
        return QApplication::applicationDirPath() + kCellResourcesFilePath;
    }
    static QString CellResourcesDirPath() {
        return QApplication::applicationDirPath() + kCellResourcesDirPath;
    }

    static QString SavesDir() {
        return QApplication::applicationDirPath() + kSavesDir;
    }

    class JSON {
       public:
        class Cell {
           public:
            inline static const QString kBackgroundFileKey{
                "default_cell_background_file_name"};
            inline static const QString kFrameFileKey{
                "default_cell_frame_file_name"};
            inline static const QString kActiveFrameFileKey{
                "active_cell_frame_file_name"};
        };
        class CellLayer {
           public:
            inline static const QString kNameKey{"name"};
            inline static const QString kFileKey{"file_name"};
        };
    };
    class CellItem {
       public:
        inline static const int kDefaultCellItemSize{50};
        inline static const QColor kDefaultCellItemBackgroundColor{Qt::gray};
        inline static const bool kUseDefaultBackgroundLayer{false};
        inline static const QString kDefaultBackgroundLayerName{"Grass"};
    };

   protected:
    inline static const QString kCellLayerResourcesFilePath{
        "/resources/cellLayer.json"};
    inline static const QString kCellLayerResourcesDirPath{
        "/resources/cellLayer_res/"};

    inline static const QString kCellResourcesFilePath{"/resources/cell.json"};
    inline static const QString kCellResourcesDirPath{"/resources/cell_res/"};

    inline static const QString kSavesDir{"/saves/"};
};
