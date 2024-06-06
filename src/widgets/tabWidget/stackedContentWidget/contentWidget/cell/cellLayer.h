#pragma once

#include <qassert.h>
#include <qdebug.h>
#include <qfile.h>
#include <qhash.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qmap.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qstringview.h>

#include <cassert>
#include <utility>

#include "../../../../../settings.h"

class CellLayer {
   public:
    const QPixmap* pixmap() const { return pixmap_; }

    QString name() const { return name_; }

    bool isEmpty() const { return pixmap_ == nullptr; }

    CellLayer(CellLayer&&) = delete;

   protected:
    CellLayer(QString name, QString file_path)
        : name_{std::move(name)}, file_path_{std::move(file_path)} {
        pixmap_ = new QPixmap{file_path_};
    }
    explicit CellLayer(QString name) : name_{std::move(name)} {}
    ~CellLayer() { delete pixmap_; }

    CellLayer& operator=(const CellLayer& other) {
        if (this == &other) {
            return *this;
        }

        name_ = other.name_;
        file_path_ = other.file_path_;
        *pixmap_ = *other.pixmap_;

        return *this;
    }
    bool operator==(const CellLayer& other) const = default;

    QString name_;
    // TODO(clovis): delete this member variable?
    QString file_path_;
    QPixmap* pixmap_{nullptr};
};

class Liquid : public CellLayer {
   public:
    static const Liquid* get(const QString& name) {
        if (k_liquids_.contains(name)) {
            return k_liquids_[name];
        }

        return nullptr;
    };

    // TODO(clovis): fix clang-tidy warning
    // NOLINTNEXTLINE
    static const QMap<int, Liquid*> list() { return k_liquids_sorted_; };

    static void add(const QString& name, const QString& file_path) {
        if (!QFile::exists(file_path)) {
            qDebug() << "Failed to create pixmap: file does not exists";
            return;
        }

        // If exists
        if (k_liquids_.contains(name)) {
            *k_liquids_[name] = Liquid{name, file_path};
            return;
        }

        // If does not exists
        static int id{0};

        insert_item(++id, new Liquid{name, file_path});
    }
    static void add() {
        // If exists
        if (k_liquids_.contains(kNoLiquidName)) {
            return;
        }

        // If does not exists
        insert_item(0, new Liquid{});
    }
    static void clear() {
        for (auto* i : k_liquids_) {
            delete i;
        }

        k_liquids_.clear();
        k_liquids_sorted_.clear();
    }

   protected:
    Liquid(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {}
    Liquid() : CellLayer(kNoLiquidName) {}

    static void insert_item(int id, Liquid* liquid) {
        k_liquids_sorted_.insert(id, liquid);
        k_liquids_.insert(liquid->name(), liquid);
    }
    static void remove_item(const QString& name) {
        if (k_liquids_.contains(name)) {
            k_liquids_sorted_.remove(k_liquids_sorted_.key(k_liquids_[name]));

            delete k_liquids_[name];

            k_liquids_.remove(name);
        }
    }

    inline static QHash<QString, Liquid*> k_liquids_{};
    inline static QMap<int, Liquid*> k_liquids_sorted_{};
    inline static const QString kNoLiquidName{"No liquid"};

   public:
    inline static const QString kJsonName{"Liquid"};
};

class Gaz : public CellLayer {
   public:
    static const Gaz* get(const QString& name) {
        if (k_gasses_.contains(name)) {
            return k_gasses_[name];
        }

        return nullptr;
    };

    // TODO(clovis): fix clang-tidy warning
    // NOLINTNEXTLINE
    static const QMap<int, Gaz*> list() { return k_gasses_sorted_; };

    static void add(const QString& name, const QString& file_path) {
        if (!QFile::exists(file_path)) {
            qDebug() << "Failed to create pixmap: file does not exists";
            return;
        }

        // If exists
        if (k_gasses_.contains(name)) {
            *k_gasses_[name] = Gaz{name, file_path};
            return;
        }

        // If does not exists
        static int id{0};

        insert_item(++id, new Gaz{name, file_path});
    }
    static void add() {
        // If exists
        if (k_gasses_.contains(kNoGazName)) {
            return;
        }

        // If does not exists
        insert_item(0, new Gaz{});
    }
    static void clear() {
        for (auto* i : k_gasses_) {
            delete i;
        }

        k_gasses_.clear();
        k_gasses_sorted_.clear();
    }

   protected:
    Gaz(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {}
    Gaz() : CellLayer(kNoGazName) {}

    static void insert_item(int id, Gaz* gaz) {
        k_gasses_sorted_.insert(id, gaz);
        k_gasses_.insert(gaz->name(), gaz);
    }
    static void remove_item(const QString& name) {
        if (k_gasses_.contains(name)) {
            k_gasses_sorted_.remove(k_gasses_sorted_.key(k_gasses_[name]));

            delete k_gasses_[name];

            k_gasses_.remove(name);
        }
    }

    inline static QHash<QString, Gaz*> k_gasses_{};
    inline static QMap<int, Gaz*> k_gasses_sorted_{};
    inline static const QString kNoGazName{"No gaz"};

   public:
    inline static const QString kJsonName{"Gaz"};
};

class Background : public CellLayer {
   public:
    static const Background* get(const QString& name) {
        if (k_backgrounds_.contains(name)) {
            return k_backgrounds_[name];
        }

        return nullptr;
    };

    // TODO(clovis): fix clang-tidy warning
    // NOLINTNEXTLINE
    static const QMap<int, Background*> list() {
        return k_backgrounds_sorted_;
    };

    static void add(const QString& name, const QString& file_path) {
        if (!QFile::exists(file_path)) {
            qDebug() << "Failed to create pixmap: file does not exists";
            return;
        }

        // If exists
        if (k_backgrounds_.contains(name)) {
            *k_backgrounds_[name] = Background{name, file_path};
            return;
        }

        // If does not exists
        static int id{0};

        insert_item(++id, new Background{name, file_path});
    }
    static void add() {
        // If exists
        if (k_backgrounds_.contains(kNoBackgroundName)) {
            return;
        }

        // If does not exists
        insert_item(0, new Background{});
    }
    static void clear() {
        for (auto* i : k_backgrounds_) {
            delete i;
        }

        k_backgrounds_.clear();
        k_backgrounds_sorted_.clear();
    }

   protected:
    Background(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {}
    Background() : CellLayer(kNoBackgroundName) {}

    static void insert_item(int id, Background* background) {
        k_backgrounds_sorted_.insert(id, background);
        k_backgrounds_.insert(background->name(), background);
    }
    static void remove_item(const QString& name) {
        if (k_backgrounds_.contains(name)) {
            k_backgrounds_sorted_.remove(
                k_backgrounds_sorted_.key(k_backgrounds_[name]));

            delete k_backgrounds_[name];

            k_backgrounds_.remove(name);
        }
    }

    inline static QHash<QString, Background*> k_backgrounds_{};
    inline static QMap<int, Background*> k_backgrounds_sorted_{};
    inline static const QString kNoBackgroundName{"No background"};

   public:
    inline static const QString kJsonName{"Background"};
};

inline void loadCellLayersFromJson() {
    QFile res{Settings::kCellLayerResourcesFilePath};
    if (!res.exists()) {
        qDebug() << res.fileName() << "does not exists";

        return;
    }

    if (res.open(QIODevice::ReadOnly)) {
        QByteArray bytes{res.readAll()};
        res.close();

        QJsonParseError json_error{};
        QJsonDocument doc{QJsonDocument::fromJson(bytes, &json_error)};

        if (doc.isObject()) {
            QJsonObject json{doc.object()};

            // Background
            Background::clear();

            Background::add();
            for (QJsonValue i : json[Background::kJsonName].toArray()) {
                QString name{i[Settings::JSON::CellLayer::kNameKey].toString()};
                QString file_name{
                    i[Settings::JSON::CellLayer::kFileKey].toString()};

                if (name.isEmpty() || file_name.isEmpty()) {
                    continue;
                }
                QFile file{Settings::kCellLayerResourcesDirPath + file_name};
                if (!file.exists()) {
                    continue;
                }

                Background::add(name, file.fileName());
            }

            // Liquid
            Liquid::clear();

            Liquid::add();
            for (QJsonValue i : json[Liquid::kJsonName].toArray()) {
                QString name{i[Settings::JSON::CellLayer::kNameKey].toString()};
                QString file_name{
                    i[Settings::JSON::CellLayer::kFileKey].toString()};

                if (name.isEmpty() || file_name.isEmpty()) {
                    continue;
                }
                QFile file{Settings::kCellLayerResourcesDirPath + file_name};
                if (!file.exists()) {
                    continue;
                }

                Liquid::add(name, file.fileName());
            }
            // Gaz
            Gaz::clear();

            Gaz::add();
            for (QJsonValue i : json[Gaz::kJsonName].toArray()) {
                QString name{i[Settings::JSON::CellLayer::kNameKey].toString()};
                QString file_name{
                    i[Settings::JSON::CellLayer::kFileKey].toString()};

                if (name.isEmpty() || file_name.isEmpty()) {
                    continue;
                }
                QFile file{Settings::kCellLayerResourcesDirPath + file_name};
                if (!file.exists()) {
                    continue;
                }

                Gaz::add(name, file.fileName());
            }
        }
    } else {
        qDebug() << res.fileName() << "Error: " << res.errorString();
    }
}