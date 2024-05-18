#pragma once

#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "celllayer.h"

class Cell : public QLabel {
    Q_OBJECT

   public:
    explicit Cell(QWidget* parent, int cell_size = kCellSize,
                  QColor background_color = kCellBackgroundColor)
        : QLabel{parent},
          background_color_{background_color},
          size_{cell_size},
          pixmap_{new QPixmap{size_, size_}} {
        setFixedSize(kCellSize, kCellSize);
        drawCell();

        QObject::connect(this, &Cell::clicked, this, &Cell::onClicked);
    }

    void setLiquid(Liquid* liquid) { liquid_ = liquid; }
    void setGaz(Gaz* gaz) { gaz_ = gaz; }

   signals:
    void clicked();

   private slots:
    void onClicked() {
        setLiquid(Liquid::getLiquid("Oil"));
        setGaz(Gaz::getGaz("Poison"));

        drawCell();
    }

   private:
    // Draws cells depending on its member variables.
    void drawCell() {
        // Background
        pixmap_->fill(background_color_);

        QPainter pntr{pixmap_};

        // Liquid
        if (liquid_ != nullptr) {
            pntr.drawPixmap(0, 0, liquid_->pixmap()->scaled(size_, size_));
        }
        // Gaz
        if (gaz_ != nullptr) {
            pntr.drawPixmap(0, 0, gaz_->pixmap()->scaled(size_, size_));
        }

        setPixmap(*pixmap_);
    }
    void mousePressEvent(QMouseEvent* /*e*/) override { emit clicked(); }

    QColor background_color_;
    int size_{};
    QPixmap* pixmap_;
    Liquid* liquid_{nullptr};
    Gaz* gaz_{nullptr};

    inline static constexpr int kCellSize{50};
    inline static constexpr QColor kCellBackgroundColor{200, 200, 200};
};