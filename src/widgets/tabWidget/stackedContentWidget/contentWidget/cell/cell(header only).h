#pragma once

#include <qcoreevent.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlist.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "../../../../../tool.h"
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

    struct CellInfo {
        QString background;
        QString liquid;
        QString gaz;
        // bool selected{false};

        bool operator==(const CellInfo& other) const = default;
    };

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
                QString active_frame{
                    json[Settings::JSON::Cell::kActiveFrameFileKey].toString()};

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
                // Active Frame
                if (active_frame.isEmpty()) {
                    qDebug() << "Failed to find value of"
                             << Settings::JSON::Cell::kActiveFrameFileKey
                             << "in" << Settings::kCellResourcesFilePath;
                } else {
                    QString active_frame_path{Settings::kCellResourcesDirPath +
                                              active_frame};

                    if (QFile::exists(active_frame_path)) {
                        delete k_active_cell_frame_;
                        k_active_cell_frame_ = new QPixmap{active_frame_path};
                    } else {
                        qDebug() << active_frame_path << "not found";
                    }
                }
            }
        } else {
            qDebug() << res.fileName() << "Error: " << res.errorString();
        }
    };

    void setSelected(bool selected = true) {
        selected_ = selected;
        drawCell();
    }

    void setLayer(const Liquid* liquid, bool track_history = true) {
        if (liquid) {
            liquid_ = liquid->name();
            drawCell();
        }

        if (track_history) {
            // TODO(clovis): implement history action here
        }
    }
    void setLayer(const Gaz* gaz, bool track_history = true) {
        if (gaz) {
            gaz_ = gaz->name();
            drawCell();
        }

        if (track_history) {
            // TODO(clovis): implement history action here
        }
    }
    void setLayer(const Background* background, bool track_history = true) {
        if (background) {
            background_ = background->name();
            drawCell();
        }

        if (track_history) {
            // TODO(clovis): implement history action here
        }
    }
    void setLayer(const CellInfo& i) {
        if (i == info()) {
            return;
        }

        background_ = i.background;
        liquid_ = i.liquid;
        gaz_ = i.gaz;

        drawCell();
    }
    void clearLayers(bool track_history = true) {
        liquid_.clear();
        gaz_.clear();
        drawCell();

        if (track_history) {
            // TODO(clovis): implement history action here
        }
    }

    CellInfo info() { return CellInfo{background_, liquid_, gaz_}; }

    QString background() { return background_; }
    QString liquid() { return liquid_; }
    QString gaz() { return gaz_; }

   signals:
    void clicked();

   private slots:
    void onClicked() {
        switch (Tool::toolType()) {
            case Tool::kLiquid:
                setLayer(static_cast<const Liquid*>(Tool::cell_layer()));
                break;
            case Tool::kGaz:
                setLayer(static_cast<const Gaz*>(Tool::cell_layer()));
                break;
            case Tool::kClear:
                clearLayers();
                break;
            case Tool::kBackground:
                setLayer(static_cast<const Background*>(Tool::cell_layer()));
                break;
            case Tool::kNone:
            case Tool::kEnumLength:
                break;
        }
    }

   private:
    // Draws cells depending on its member variables.
    void drawCell() {
        // Background
        const auto* background{Background::get(background_)};
        if (background == nullptr || background->isEmpty()) {
            if (k_use_default_background_ && k_default_background_ != nullptr)
                *pixmap_ = k_default_background_->scaled(size_, size_);
            else
                pixmap_->fill(background_color_);
        } else {
            *pixmap_ = background->pixmap()->scaled(size_, size_);
        }

        QPainter pntr{pixmap_};

        // Frame
        if (selected_) {
            if (k_active_cell_frame_ != nullptr) {
                pntr.drawPixmap(0, 0,
                                k_active_cell_frame_->scaled(size_, size_));
            }
        } else {
            if (k_cell_frame_ != nullptr) {
                pntr.drawPixmap(0, 0, k_cell_frame_->scaled(size_, size_));
            }
        }

        // Liquid
        const auto* liquid{Liquid::get(liquid_)};
        if (liquid != nullptr && !liquid->isEmpty()) {
            pntr.drawPixmap(0, 0, liquid->pixmap()->scaled(size_, size_));
        }

        // Gaz
        const auto* gaz{Gaz::get(gaz_)};
        if (gaz != nullptr && !gaz->isEmpty()) {
            pntr.drawPixmap(0, 0, gaz->pixmap()->scaled(size_, size_));
        }

        setPixmap(*pixmap_);
    }

    void mousePressEvent(QMouseEvent* e) override {
        emit clicked();

        QLabel::mousePressEvent(e);
    }

    QColor background_color_;
    int size_{};
    QPixmap* pixmap_;
    QString background_;
    QString liquid_;
    QString gaz_;
    bool selected_{false};

    inline static QPixmap* k_default_background_{nullptr};
    inline static QPixmap* k_cell_frame_{nullptr};
    inline static QPixmap* k_active_cell_frame_{nullptr};
    inline static constexpr int kCellSize{50};
    inline static constexpr QColor kDefaultBackgroundColor{200, 200, 200};
    inline static bool k_use_default_background_{};
};
