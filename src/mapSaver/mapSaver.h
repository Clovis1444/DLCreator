#pragma once

#include <qcontainerfwd.h>
#include <qjsondocument.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cellCollection.h"

class MapSaver : public QObject {
    Q_OBJECT

   public:
    static QJsonDocument saveMapToFile(const CellCollection* cc);

    static CellCollection* loadMapFromFile(const QString& file_name,
                                           const QByteArray& file_content);

    static QString getSaveFilePath(const QString& name,
                                   bool add_number = false);

    inline static const QString kRootObjKey{
        Settings::kSaveFileExtension.sliced(1)};
    inline static const QString kFileCommentKey{"comment"};

    inline static const QString kRootMapSizeKey{"map_size"};
    inline static const QString kRootCellsKey{"Cells"};
    inline static const QString kCellSelectedKey{"Selected"};
    inline static const QString kCellPosKey{"Pos"};
};