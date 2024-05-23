#pragma once

#include <qassert.h>
#include <qdebug.h>
#include <qfile.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qstringview.h>

#include <cassert>
#include <utility>

#include "../../settings.h"

class CellLayer {
   public:
    const QPixmap* pixmap() const { return pixmap_; }

    QString name() const { return name_; }

    bool isEmpty() const { return pixmap_ == nullptr; }

    CellLayer(CellLayer&&) = delete;

   protected:
    CellLayer(QString name, QString file_path)
        : name_{std::move(name)}, file_path_{std::move(file_path)} {
        // If file exists - create pixmap
        if (!QFile::exists(file_path_)) {
            qDebug() << "Failed to create pixmap: file does not exists";
            static_assert(true);
        }
        pixmap_ = new QPixmap{file_path_};
    }
    explicit CellLayer(QString name) : name_{std::move(name)} {}
    ~CellLayer() { delete pixmap_; }

    QString name_;
    // TODO(clovis): delete this member variable?
    QString file_path_;
    QPixmap* pixmap_{nullptr};
};

class Liquid : public CellLayer {
   public:
    static const Liquid* getLiquid(const QString& name) {
        for (Liquid* i : k_liquids_) {
            if (i->name_ == name) {
                return i;
            }
        }

        return nullptr;
    };

    // TODO(clovis): fix clang-tidy warning
    // NOLINTNEXTLINE
    static const QList<Liquid*> list() { return k_liquids_; };

   protected:
    Liquid(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {
        k_liquids_.push_back(this);
    }
    Liquid() : CellLayer("No liquid") { k_liquids_.push_back(this); }

    inline static QList<Liquid*> k_liquids_{};
    inline static const QString kJsonName{"Liquid"};

    friend void loadCellLayersFromJson();
};

class Gaz : public CellLayer {
   public:
    static const Gaz* getGaz(const QString& name) {
        for (Gaz* i : k_gasses_) {
            if (i->name_ == name) {
                return i;
            }
        }

        return nullptr;
    };

    // TODO(clovis): fix clang-tidy warning
    // NOLINTNEXTLINE
    static const QList<Gaz*> list() { return k_gasses_; };

   protected:
    Gaz(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {
        k_gasses_.push_back(this);
    }
    Gaz() : CellLayer("No Gaz") { k_gasses_.push_back(this); }

    inline static QList<Gaz*> k_gasses_{};
    inline static const QString kJsonName{"Gaz"};

    friend void loadCellLayersFromJson();
};

class Background : public CellLayer {
   public:
    static const Background* getBackground(const QString& name) {
        for (Background* i : k_backgrounds_) {
            if (i->name_ == name) {
                return i;
            }
        }

        return nullptr;
    };

    // TODO(clovis): fix clang-tidy warning
    // NOLINTNEXTLINE
    static const QList<Background*> list() { return k_backgrounds_; };

   protected:
    Background(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {
        k_backgrounds_.push_back(this);
    }
    Background() : CellLayer("No Background") {
        k_backgrounds_.push_back(this);
    }

    inline static QList<Background*> k_backgrounds_{};
    inline static const QString kJsonName{"Background"};

    friend void loadCellLayersFromJson();
};

// Parse json and appends its objects to associated QList.
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
            new Background{};
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

                new Background{name, file.fileName()};
            }
            // Liquid
            new Liquid{};
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

                new Liquid{name, file.fileName()};
            }
            // Gaz
            new Gaz{};
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

                new Gaz{name, file.fileName()};
            }
        }
    } else {
        qDebug() << res.fileName() << "Error: " << res.errorString();
    }
}