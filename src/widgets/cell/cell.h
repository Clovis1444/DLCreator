#pragma once

#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "../../tool.h"
#include "cellLayer.h"

class Cell : public QLabel {
    Q_OBJECT

   public:
    explicit Cell(QWidget* parent, int cell_size = kCellSize,
                  QColor background_color = kDefaultBackgroundColor)
        : QLabel{parent},
          background_color_{background_color},
          size_{cell_size},
          pixmap_{new QPixmap{size_, size_}} {
        setFixedSize(kCellSize, kCellSize);
        drawCell();

        QObject::connect(this, &Cell::clicked, this, &Cell::onClicked);
    }

    static void loadResourcesFromJson() {
        QFile res{Settings::kCellResourcesFilePath};
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

                QString background{
                    json[Settings::JSON::Cell::kBackgroundFileKey].toString()};
                QString frame{
                    json[Settings::JSON::Cell::kFrameFileKey].toString()};

                // Background
                if (background.isEmpty()) {
                    qDebug() << "Failed to find value of"
                             << Settings::JSON::Cell::kBackgroundFileKey << "in"
                             << Settings::kCellResourcesFilePath;
                } else {
                    QString background_path{Settings::kCellResourcesDirPath +
                                            background};

                    if (QFile::exists(background_path)) {
                        delete k_default_background_;
                        k_default_background_ = new QPixmap{background_path};
                    } else {
                        qDebug() << background_path << "not found";
                    }
                }

                // Frame
                if (frame.isEmpty()) {
                    qDebug() << "Failed to find value of"
                             << Settings::JSON::Cell::kFrameFileKey << "in"
                             << Settings::kCellResourcesFilePath;
                } else {
                    QString frame_path{Settings::kCellResourcesDirPath + frame};

                    if (QFile::exists(frame_path)) {
                        delete k_cell_frame_;
                        k_cell_frame_ = new QPixmap{frame_path};
                    } else {
                        qDebug() << frame_path << "not found";
                    }
                }
            }
        } else {
            qDebug() << res.fileName() << "Error: " << res.errorString();
        }
    };

   signals:
    void clicked();

   private slots:
    void onClicked() {
        switch (Tool::toolType()) {
            case Tool::kLiquid:
                setLayer(static_cast<const Liquid*>(Tool::cell_layer()));
                drawCell();
                break;
            case Tool::kGaz:
                setLayer(static_cast<const Gaz*>(Tool::cell_layer()));
                drawCell();
                break;
            case Tool::kClear:
                clearAllLayers();
                drawCell();
                break;
            case Tool::kBackground:
                setLayer(static_cast<const Background*>(Tool::cell_layer()));
                drawCell();
                break;
            case Tool::kNone:
            case Tool::kEnumLength:
                break;
        }
    }

   private:
    void clearAllLayers() {
        liquid_ = nullptr;
        gaz_ = nullptr;
    }
    void setLayer(const Liquid* liquid) { liquid_ = liquid; }
    void setLayer(const Gaz* gaz) { gaz_ = gaz; }
    void setLayer(const Background* background) { background_ = background; }

    // Draws cells depending on its member variables.
    void drawCell() {
        // Background
        if (background_ == nullptr || background_->isEmpty()) {
            if (k_use_default_background_ && k_default_background_ != nullptr)
                *pixmap_ = k_default_background_->scaled(size_, size_);
            else
                pixmap_->fill(background_color_);
        } else {
            *pixmap_ = background_->pixmap()->scaled(size_, size_);
        }

        QPainter pntr{pixmap_};

        // Frame
        if (k_cell_frame_ != nullptr) {
            pntr.drawPixmap(0, 0, k_cell_frame_->scaled(size_, size_));
        }

        // Liquid
        if (liquid_ != nullptr && !liquid_->isEmpty()) {
            pntr.drawPixmap(0, 0, liquid_->pixmap()->scaled(size_, size_));
        }

        // Gaz
        if (gaz_ != nullptr && !gaz_->isEmpty()) {
            pntr.drawPixmap(0, 0, gaz_->pixmap()->scaled(size_, size_));
        }

        setPixmap(*pixmap_);
    }
    void mousePressEvent(QMouseEvent* /*e*/) override { emit clicked(); }

    QColor background_color_;
    int size_{};
    QPixmap* pixmap_;
    const Background* background_{nullptr};
    const Liquid* liquid_{nullptr};
    const Gaz* gaz_{nullptr};

    inline static QPixmap* k_default_background_{nullptr};
    inline static QPixmap* k_cell_frame_{nullptr};
    inline static constexpr int kCellSize{50};
    inline static constexpr QColor kDefaultBackgroundColor{200, 200, 200};
    inline static bool k_use_default_background_{};
};