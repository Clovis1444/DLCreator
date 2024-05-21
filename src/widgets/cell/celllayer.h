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
        for (Gaz* i : k_gazes_) {
            if (i->name_ == name) {
                return i;
            }
        }

        return nullptr;
    };

    // TODO(clovis): fix clang-tidy warning
    // NOLINTNEXTLINE
    static const QList<Gaz*> list() { return k_gazes_; };

   protected:
    Gaz(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {
        k_gazes_.push_back(this);
    }
    Gaz() : CellLayer("No Gaz") { k_gazes_.push_back(this); }

    inline static QList<Gaz*> k_gazes_{};
    inline static const QString kJsonName{"Gaz"};

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

            // Liquid
            // TODO(clovis): refactor this
            new Liquid{};
            for (QJsonValue i : json[Liquid::kJsonName].toArray()) {
                QString name{i["name"].toString()};
                QString file_name{i["file_name"].toString()};

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
            // TODO(clovis): refactor this
            new Gaz{};
            for (QJsonValue i : json[Gaz::kJsonName].toArray()) {
                QString name{i["name"].toString()};
                QString file_name{i["file_name"].toString()};

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