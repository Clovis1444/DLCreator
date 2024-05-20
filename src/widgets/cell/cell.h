#pragma once

#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "../../tool.h"
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

    void setLayer(const Liquid* liquid) { liquid_ = liquid; }
    void setLayer(const Gaz* gaz) { gaz_ = gaz; }

   signals:
    void clicked();

   private slots:
    void onClicked() { updateUsingActiveTool(); }

   private:
    void updateUsingActiveTool() {
        switch (Tool::toolType()) {
            case Tool::kLiquid:
                setLayer(static_cast<const Liquid*>(Tool::cell_layer()));
                drawCell();
                break;
            case Tool::kGaz:
                setLayer(static_cast<const Gaz*>(Tool::cell_layer()));
                drawCell();
                break;
            case Tool::kNone:
            case Tool::kEnumLength:
                break;
        }
    }
    // Draws cells depending on its member variables.
    void drawCell() {
        // Background
        pixmap_->fill(background_color_);

        QPainter pntr{pixmap_};

        // Liquid
        if (liquid_ != nullptr && liquid_->pixmap() != nullptr) {
            pntr.drawPixmap(0, 0, liquid_->pixmap()->scaled(size_, size_));
        }
        // Gaz
        if (gaz_ != nullptr && gaz_->pixmap() != nullptr) {
            pntr.drawPixmap(0, 0, gaz_->pixmap()->scaled(size_, size_));
        }

        setPixmap(*pixmap_);
    }
    void mousePressEvent(QMouseEvent* /*e*/) override { emit clicked(); }

    QColor background_color_;
    int size_{};
    QPixmap* pixmap_;
    const Liquid* liquid_{nullptr};
    const Gaz* gaz_{nullptr};

    inline static constexpr int kCellSize{50};
    inline static constexpr QColor kCellBackgroundColor{200, 200, 200};
};