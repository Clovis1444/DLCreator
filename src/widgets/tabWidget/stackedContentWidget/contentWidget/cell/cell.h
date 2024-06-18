#pragma once

#include <qcoreevent.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlist.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>

#include "cellLayer.h"

class Cell : public QLabel {
    Q_OBJECT

   public:
    explicit Cell(QWidget* parent, int cell_size = kCellSize,
                  QColor background_color = kDefaultBackgroundColor);

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

    void setSelected(bool selected = true);

    void setLayer(const CellLayer* layer, bool track_history = true);
    void setLayer(const CellInfo& i);
    void clearLayers(bool track_history = true);
    CellInfo info();

    QString background() const;
    QString liquid() const;
    QString gaz() const;

   signals:
    void clicked();

   private slots:
    void onClicked();

   private:
    // Draws cells depending on its member variables.
    void drawCell();

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    QColor background_color_;
    int size_{};
    QPixmap* pixmap_;
    QString background_;
    QString liquid_;
    QString gaz_;
    bool selected_{false};

    QPoint click_pos_;

    inline static QPixmap* k_default_background_{nullptr};
    inline static QPixmap* k_cell_frame_{nullptr};
    inline static QPixmap* k_active_cell_frame_{nullptr};
    inline static constexpr int kCellSize{50};
    inline static constexpr QColor kDefaultBackgroundColor{200, 200, 200};
    inline static bool k_use_default_background_{};
};