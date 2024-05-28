#pragma once

#include <qapplication.h>
#include <qevent.h>
#include <qgraphicsview.h>
#include <qlayout.h>
#include <qobject.h>
#include <qpainter.h>
#include <qrubberband.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qwidget.h>

#include <QWheelEvent>

#include "../tool.h"

class CellCollection : public QWidget {
    Q_OBJECT

   public:
    explicit CellCollection(QWidget* parent) : QWidget{parent} {}

   protected:
    // TODO(clovis): implement zoom
    void wheelEvent(QWheelEvent* e) override {
        if (e->angleDelta().y() < 0) {
            if (scale_ > 0) scale_ -= kScaleStep;
        } else {
            scale_ += kScaleStep;
        };

        if (scale_ > kMaxScale) scale_ = kMaxScale;
        if (scale_ < kMinScale) scale_ = kMinScale;

        // qDebug() << scale_;
    };

    void mousePressEvent(QMouseEvent* e) override {
        if (Tool::toolType() == Tool::ToolType::kNone) {
            selection_begin_ = e->pos();

            selection_->setGeometry(QRect(selection_begin_, QSize()));
            selection_->show();
        }
    }
    void mouseMoveEvent(QMouseEvent* e) override {
        selection_->setGeometry(QRect(selection_begin_, e->pos()).normalized());
    }
    void mouseReleaseEvent(QMouseEvent* /*e*/) override {
        selection_->hide();
        // determine selection, for example using QRect::intersects()
        // and QRect::contains().
    }

   private:
    int scale_{1};

    QRubberBand* selection_{new QRubberBand(QRubberBand::Rectangle, this)};
    QPoint selection_begin_;

    inline static constexpr int kMaxScale{10};
    inline static constexpr int kMinScale{-10};
    inline static constexpr int kScaleStep{1};
};