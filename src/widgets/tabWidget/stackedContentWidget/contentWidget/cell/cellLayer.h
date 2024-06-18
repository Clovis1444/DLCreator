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
#include <qvarlengtharray.h>

#include <cassert>
#include <utility>

#include "../../../../../settings.h"

class CellLayer {
   public:
    enum Type {
        kBackground,
        kLiquid,
        kGaz,
        // Insert new types here
        kEnumLength
    };

    static const CellLayer* get(Type type, const QString& name) {
        if (layers_[type].contains(name)) {
            return layers_[type][name];
        }

        return nullptr;
    };

    static const QList<CellLayer*>& list(Type type) {
        return layers_sorted_[type];
    };

    static void add(Type type, const QString& name, const QString& file_path) {
        if (!QFile::exists(file_path)) {
            qDebug() << "Failed to create pixmap: file does not exists";
            return;
        }

        // If exists
        if (layers_[type].contains(name)) {
            *layers_[type][name] = CellLayer{type, name, file_path};
            return;
        }

        // If does not exists
        insert_layer(new CellLayer{type, name, file_path});
    }
    static void add(Type type) {
        // If exists
        if (layers_[type].contains(noName(type))) {
            return;
        }
        // If does not exists
        insert_layer(new CellLayer{type});
    }

    static void clear() {
        for (int i{0}; i < Type::kEnumLength; ++i) {
            clear(static_cast<Type>(i));
        }
    }
    static void clear(Type type) {
        if (type == Type::kEnumLength) {
            return;
        }

        for (auto* i : layers_[type]) {
            delete i;
        }

        layers_[type].clear();
        layers_sorted_[type].clear();
    }

    const QPixmap* pixmap() const { return pixmap_; }

    Type type() const { return type_; }

    QString name() const { return name_; }

    bool isEmpty() const { return pixmap_ == nullptr; }

    inline static QString jsonName(Type type) { return kJsonNames[type]; }

    inline static void loadCellLayersFromJson() {
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

                CellLayer::clear();

                for (int t{}; t < CellLayer::Type::kEnumLength; ++t) {
                    const auto type{static_cast<CellLayer::Type>(t)};

                    CellLayer::add(type);

                    for (QJsonValue i :
                         json[CellLayer::jsonName(type)].toArray()) {
                        QString name{
                            i[Settings::JSON::CellLayer::kNameKey].toString()};
                        QString file_name{
                            i[Settings::JSON::CellLayer::kFileKey].toString()};

                        if (name.isEmpty() || file_name.isEmpty()) {
                            continue;
                        }
                        QFile file{Settings::kCellLayerResourcesDirPath +
                                   file_name};
                        if (!file.exists()) {
                            continue;
                        }

                        CellLayer::add(type, name, file.fileName());
                    }
                }
            }
        } else {
            qDebug() << res.fileName() << "Error: " << res.errorString();
        }
    }

    CellLayer(CellLayer&&) = delete;

   protected:
    CellLayer(Type type, QString name, QString file_path)
        : type_{type},
          name_{std::move(name)},
          file_path_{std::move(file_path)} {
        pixmap_ = new QPixmap{file_path_};
    }
    explicit CellLayer(Type type) : type_{type}, name_{noName(type)} {}
    ~CellLayer() { delete pixmap_; }

    static void insert_layer(CellLayer* layer) {
        if (layer->type() == Type::kEnumLength) {
            return;
        }

        layers_sorted_[layer->type()].push_back(layer);
        layers_[layer->type()].insert(layer->name(), layer);
    }

    static void remove_item(Type type, const QString& name) {
        if (layers_[type].contains(name)) {
            layers_sorted_[type].removeOne(layers_[type][name]);

            delete layers_[type][name];

            layers_[type].remove(name);
        }
    }

    inline static QString noName(Type type) { return kNoLayerNames[type]; }

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

    Type type_;
    QString name_;
    QString file_path_;
    QPixmap* pixmap_{nullptr};

    inline static const QVarLengthArray<QString, Type::kEnumLength> kJsonNames{
        "Background", "Liquid", "Gaz", /* Insert new types here */ "?"};
    inline static const QVarLengthArray<QString, Type::kEnumLength>
        kNoLayerNames{"No background", "No liquid", "No gaz",
                      /* Insert new types here */ "?"};

    inline static QVarLengthArray<QHash<QString, CellLayer*>, Type::kEnumLength>
        layers_{Type::kEnumLength, QHash<QString, CellLayer*>{}};
    inline static QVarLengthArray<QList<CellLayer*>, Type::kEnumLength>
        layers_sorted_{Type::kEnumLength, QList<CellLayer*>{}};
};
