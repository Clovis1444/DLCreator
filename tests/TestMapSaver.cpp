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
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cellCollection.h"

class TestMapSaver : public QObject {
    Q_OBJECT
   private slots:
    // Loads valid dlmap file
    static void loadValid() {
        auto* cc{loadMap(QApplication::applicationDirPath() + k10x10)};

        QVERIFY2(cc != nullptr, "Map was not loaded");

        delete cc;
    }
    // Loads invalid dlmap file
    static void loadInvalidFile() {
        auto* cc{loadMap(QApplication::applicationDirPath() + kInvalidFile)};

        QVERIFY2(cc == nullptr, "Invalid files should not be loaded");

        delete cc;
    }
    // Loads multiple dlmaps and checks their sizes
    static void checkSize() {
        auto* cc10x10{loadMap(QApplication::applicationDirPath() + k10x10)};
        QVERIFY2(cc10x10 != nullptr, "Map was not loaded");

        auto* cc2x6{loadMap(QApplication::applicationDirPath() + k2x6)};
        QVERIFY2(cc2x6 != nullptr, "Map was not loaded");

        auto* cc12x8{loadMap(QApplication::applicationDirPath() + k12x8)};
        QVERIFY2(cc12x8 != nullptr, "Map was not loaded");

        auto* cc12x11{loadMap(QApplication::applicationDirPath() + k12x11)};
        QVERIFY2(cc12x11 != nullptr, "Map was not loaded");

        const QPair<int, int> s10x10{10, 10};
        const QPair<int, int> s2x6{2, 6};
        const QPair<int, int> s12x8{12, 8};
        const QPair<int, int> s12x11{12, 11};

        QVERIFY(cc10x10->gridSize() == s10x10);
        QVERIFY(cc2x6->gridSize() == s2x6);
        QVERIFY(cc12x8->gridSize() == s12x8);
        QVERIFY(cc12x11->gridSize() == s12x11);

        delete cc10x10;
        delete cc2x6;
        delete cc12x8;
        delete cc12x11;
    };
    // Loads multiple dlmaps and checks all their cells
    static void checkAllCells() {
        auto* cc10x10{loadMap(QApplication::applicationDirPath() + k10x10)};
        QVERIFY2(cc10x10 != nullptr, "Map was not loaded");

        auto* cc2x6{loadMap(QApplication::applicationDirPath() + k2x6)};
        QVERIFY2(cc2x6 != nullptr, "Map was not loaded");

        auto* cc12x8{loadMap(QApplication::applicationDirPath() + k12x8)};
        QVERIFY2(cc12x8 != nullptr, "Map was not loaded");

        auto* cc12x11{loadMap(QApplication::applicationDirPath() + k12x11)};
        QVERIFY2(cc12x11 != nullptr, "Map was not loaded");

        QVERIFY(
            checkCells(cc10x10, QApplication::applicationDirPath() + k10x10));
        QVERIFY(checkCells(cc2x6, QApplication::applicationDirPath() + k2x6));
        QVERIFY(checkCells(cc12x8, QApplication::applicationDirPath() + k12x8));
        QVERIFY(
            checkCells(cc12x11, QApplication::applicationDirPath() + k12x11));

        delete cc10x10;
        delete cc2x6;
        delete cc12x8;
        delete cc12x11;
    }
    // Load dlmap from file -> save it as json -> match input and output
    static void testSaveMapToFile() {
        auto* cc10x10{loadMap(QApplication::applicationDirPath() + k10x10)};
        QVERIFY2(cc10x10 != nullptr, "Map was not loaded");

        auto* cc2x6{loadMap(QApplication::applicationDirPath() + k2x6)};
        QVERIFY2(cc2x6 != nullptr, "Map was not loaded");

        auto* cc12x8{loadMap(QApplication::applicationDirPath() + k12x8)};
        QVERIFY2(cc12x8 != nullptr, "Map was not loaded");

        auto* cc12x11{loadMap(QApplication::applicationDirPath() + k12x11)};
        QVERIFY2(cc12x11 != nullptr, "Map was not loaded");

        //
        QJsonDocument saved10x10{MapSaver::saveMapToFile(cc10x10)};
        QJsonDocument loaded10x10{
            loadMapAsJsonObj(QApplication::applicationDirPath() + k10x10)};
        QVERIFY(saved10x10 == loaded10x10);

        QJsonDocument saved2x6{MapSaver::saveMapToFile(cc2x6)};
        QJsonDocument loaded2x6{
            loadMapAsJsonObj(QApplication::applicationDirPath() + k2x6)};
        QVERIFY(saved2x6 == loaded2x6);

        QJsonDocument saved12x8{MapSaver::saveMapToFile(cc12x8)};
        QJsonDocument loaded12x8{
            loadMapAsJsonObj(QApplication::applicationDirPath() + k12x8)};
        QVERIFY(saved12x8 == loaded12x8);

        QJsonDocument saved12x11{MapSaver::saveMapToFile(cc12x11)};
        QJsonDocument loaded12x11{
            loadMapAsJsonObj(QApplication::applicationDirPath() + k12x11)};
        QVERIFY(saved12x11 == loaded12x11);

        delete cc10x10;
        delete cc2x6;
        delete cc12x8;
        delete cc12x11;
    }

   private:
    static CellCollection* loadMap(const QString& map_path) {
        QFile file{map_path};

        if (!file.open(QIODevice::ReadOnly)) {
            return nullptr;
        }
        QByteArray content{file.readAll()};
        file.close();

        CellCollection* cc{MapSaver::loadMapFromFile(file.fileName(), content)};
        return cc;
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
    static bool checkCells(CellCollection* cc, const QString& map_path) {
        QJsonObject obj{
            loadMapAsJsonObj(map_path)[MapSaver::kRootObjKey].toObject()};
        if (obj.isEmpty()) {
            qWarning() << "Sample map was not loaded!";
            return false;
        }

        QPair<int, int> size{
            obj[MapSaver::kRootMapSizeKey].toArray()[0].toInt(),
            obj[MapSaver::kRootMapSizeKey].toArray()[1].toInt()};

        if (cc->gridSize() != size) {
            qWarning() << "Maps sizes do not match";
            return false;
        }

        for (const auto* i : cc->cellList()) {
            QPoint pos{i->pos()};

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