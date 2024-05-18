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
    const QPixmap* pixmap() { return pixmap_; }

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
    ~CellLayer() { delete pixmap_; }

    QString name_;
    // TODO(clovis): delete this member variable?
    QString file_path_;
    QPixmap* pixmap_{nullptr};
};

class Liquid : public CellLayer {
   public:
    static Liquid* getLiquid(const QString& name) {
        for (Liquid* i : k_liquids_) {
            if (i->name_ == name) {
                return i;
            }
        }

        return nullptr;
    };

   protected:
    Liquid(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {}

    inline static QList<Liquid*> k_liquids_{};
    inline static const QString kJsonName{"Liquid"};

    friend void initCellLayersFromJson();
};

class Gaz : public CellLayer {
   public:
    static Gaz* getGaz(const QString& name) {
        for (Gaz* i : k_gazes_) {
            if (i->name_ == name) {
                return i;
            }
        }

        return nullptr;
    };

   protected:
    Gaz(QString name, QString file_path)
        : CellLayer{std::move(name), std::move(file_path)} {}

    inline static QList<Gaz*> k_gazes_{};
    inline static const QString kJsonName{"Gaz"};

    friend void initCellLayersFromJson();
};

// Parse json and appends its objects to associated QList.
inline void initCellLayersFromJson() {
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

                auto* liquid{new Liquid{name, file.fileName()}};
                Liquid::k_liquids_.push_back(liquid);
            }
            // Gaz
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

                auto* gaz{new Gaz{name, file.fileName()}};
                Gaz::k_gazes_.push_back(gaz);
            }
        }
    } else {
        qDebug() << res.fileName() << "Error: " << res.errorString();
    }
}