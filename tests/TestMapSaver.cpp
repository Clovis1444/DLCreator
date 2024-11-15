#include <qcontainerfwd.h>
#include <qdir.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qstringview.h>
#include <qtestcase.h>
#include <qtmetamacros.h>

#include <QTest>

#include "src/mapSaver/mapSaver.h"
#include "src/mapSaver/mapSaverJsons.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/gridManager.h"

class TestMapSaver : public QObject {
    Q_OBJECT
   private slots:
    // Loads valid dlmap file
    static void loadValid() {
        auto* gm{loadMap(QApplication::applicationDirPath() + k10x10)};

        QVERIFY2(gm != nullptr, "Map was not loaded");

        delete gm;
    }
    // Loads invalid dlmap file
    static void loadInvalidFile() {
        auto* gm{loadMap(QApplication::applicationDirPath() + kInvalidFile)};

        QVERIFY2(gm == nullptr, "Invalid files should not be loaded");

        delete gm;
    }
    // Loads multiple dlmaps and checks their sizes
    static void checkSize() {
        auto* gm10x10{loadMap(QApplication::applicationDirPath() + k10x10)};
        QVERIFY2(gm10x10 != nullptr, "Map was not loaded");

        auto* gm2x6{loadMap(QApplication::applicationDirPath() + k2x6)};
        QVERIFY2(gm2x6 != nullptr, "Map was not loaded");

        auto* gm12x8{loadMap(QApplication::applicationDirPath() + k12x8)};
        QVERIFY2(gm12x8 != nullptr, "Map was not loaded");

        auto* gm12x11{loadMap(QApplication::applicationDirPath() + k12x11)};
        QVERIFY2(gm12x11 != nullptr, "Map was not loaded");

        const QPair<int, int> s10x10{10, 10};
        const QPair<int, int> s2x6{2, 6};
        const QPair<int, int> s12x8{12, 8};
        const QPair<int, int> s12x11{12, 11};

        QVERIFY(gm10x10->gridSize() == s10x10);
        QVERIFY(gm2x6->gridSize() == s2x6);
        QVERIFY(gm12x8->gridSize() == s12x8);
        QVERIFY(gm12x11->gridSize() == s12x11);

        delete gm10x10;
        delete gm2x6;
        delete gm12x8;
        delete gm12x11;
    };
    // Loads multiple dlmaps and checks all their cells
    static void checkAllCells() {
        auto* gm10x10{loadMap(QApplication::applicationDirPath() + k10x10)};
        QVERIFY2(gm10x10 != nullptr, "Map was not loaded");

        auto* gm2x6{loadMap(QApplication::applicationDirPath() + k2x6)};
        QVERIFY2(gm2x6 != nullptr, "Map was not loaded");

        auto* gm12x8{loadMap(QApplication::applicationDirPath() + k12x8)};
        QVERIFY2(gm12x8 != nullptr, "Map was not loaded");

        auto* gm12x11{loadMap(QApplication::applicationDirPath() + k12x11)};
        QVERIFY2(gm12x11 != nullptr, "Map was not loaded");

        QVERIFY(
            checkCells(gm10x10, QApplication::applicationDirPath() + k10x10));
        QVERIFY(checkCells(gm2x6, QApplication::applicationDirPath() + k2x6));
        QVERIFY(checkCells(gm12x8, QApplication::applicationDirPath() + k12x8));
        QVERIFY(
            checkCells(gm12x11, QApplication::applicationDirPath() + k12x11));

        delete gm10x10;
        delete gm2x6;
        delete gm12x8;
        delete gm12x11;
    }
    // Load dlmap from file -> save it as json -> match input and output
    static void testSaveMapToFile() {
        auto* gm10x10{loadMap(QApplication::applicationDirPath() + k10x10)};
        QVERIFY2(gm10x10 != nullptr, "Map was not loaded");

        auto* gm2x6{loadMap(QApplication::applicationDirPath() + k2x6)};
        QVERIFY2(gm2x6 != nullptr, "Map was not loaded");

        auto* gm12x8{loadMap(QApplication::applicationDirPath() + k12x8)};
        QVERIFY2(gm12x8 != nullptr, "Map was not loaded");

        auto* gm12x11{loadMap(QApplication::applicationDirPath() + k12x11)};
        QVERIFY2(gm12x11 != nullptr, "Map was not loaded");

        //
        QJsonDocument saved10x10{MapSaver::saveMapToFile(gm10x10)};
        QJsonDocument loaded10x10{
            loadMapAsJsonObj(QApplication::applicationDirPath() + k10x10)};
        QVERIFY(saved10x10 == loaded10x10);

        QJsonDocument saved2x6{MapSaver::saveMapToFile(gm2x6)};
        QJsonDocument loaded2x6{
            loadMapAsJsonObj(QApplication::applicationDirPath() + k2x6)};
        QVERIFY(saved2x6 == loaded2x6);

        QJsonDocument saved12x8{MapSaver::saveMapToFile(gm12x8)};
        QJsonDocument loaded12x8{
            loadMapAsJsonObj(QApplication::applicationDirPath() + k12x8)};
        QVERIFY(saved12x8 == loaded12x8);

        QJsonDocument saved12x11{MapSaver::saveMapToFile(gm12x11)};
        QJsonDocument loaded12x11{
            loadMapAsJsonObj(QApplication::applicationDirPath() + k12x11)};
        QVERIFY(saved12x11 == loaded12x11);

        delete gm10x10;
        delete gm2x6;
        delete gm12x8;
        delete gm12x11;
    }

   private:
    static GridManager* loadMap(const QString& map_path) {
        QFile file{map_path};

        if (!file.open(QIODevice::ReadOnly)) {
            return nullptr;
        }
        QByteArray content{file.readAll()};
        file.close();

        GridManager* gm{MapSaver::loadMapFromFile(file.fileName(), content)};
        return gm;
    }
    static QJsonObject loadMapAsJsonObj(const QString& map_path) {
        QFile file{map_path};

        if (!file.open(QIODevice::ReadOnly)) {
            return QJsonObject{};
        }
        QByteArray content{file.readAll()};
        file.close();

        QJsonParseError err{};
        QJsonDocument json{QJsonDocument::fromJson(content, &err)};
        if (err.error != QJsonParseError::NoError) {
            return QJsonObject{};
        }

        if (!json.isObject()) {
            return QJsonObject{};
        }

        return json.object();
    }
    static bool checkCells(GridManager* gm, const QString& map_path) {
        QJsonObject obj{
            loadMapAsJsonObj(map_path)[MapSaver::kRootObjKey].toObject()};
        if (obj.isEmpty()) {
            qWarning() << "Sample map was not loaded!";
            return false;
        }

        QPair<int, int> size{
            obj[MapSaver::kRootMapSizeKey].toArray()[0].toInt(),
            obj[MapSaver::kRootMapSizeKey].toArray()[1].toInt()};

        if (gm->gridSize() != size) {
            qWarning() << "Maps sizes do not match";
            return false;
        }

        for (const auto* i : gm->cellItems()) {
            QPoint pos{i->gridPos()};

            QJsonObject cell_json{
                obj[MapSaver::kRootCellsKey]
                   [QString{"%1_%2"}.arg(pos.x()).arg(pos.y())]
                       .toObject()};
            if (cell_json.isEmpty()) {
                qWarning()
                    << QString{"Cell[%1_%2] was not found"}.arg(pos.x()).arg(
                           pos.y());
                return false;
            }

            MapSaverJsons::MapCellObj map_cell_obj{cell_json};

            if (map_cell_obj.cellInfo() != i->info()) {
                qWarning()
                    << QString{"Cell[%1_%2] does not match"}.arg(pos.x()).arg(
                           pos.y());
                return false;
            }
        }

        return true;
    }

    inline static const QString kSamplesPath{"/testSamples/MapSaver/"};
    inline static const QString k10x10{kSamplesPath + "10x10.dlmap"};
    inline static const QString k2x6{kSamplesPath + "2x6.dlmap"};
    inline static const QString k12x8{kSamplesPath + "12x8.dlmap"};
    inline static const QString k12x11{kSamplesPath + "12x11.dlmap"};
    inline static const QString kInvalidFile{kSamplesPath +
                                             "InvalidFile.dlmap"};
};

QTEST_MAIN(TestMapSaver)
#include "TestMapSaver.moc"
