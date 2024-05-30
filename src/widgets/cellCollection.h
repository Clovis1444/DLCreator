#pragma once

#include <qapplication.h>
#include <qevent.h>
#include <qgraphicsview.h>
#include <qgridlayout.h>
#include <qlayout.h>
#include <qlist.h>
#include <qobject.h>
#include <qpainter.h>
#include <qrubberband.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qwidget.h>

#include <QWheelEvent>

#include "../tool.h"
#include "cell/cell.h"

class CellCollection : public QWidget {
    Q_OBJECT

   public:
    explicit CellCollection(QWidget* parent) : QWidget{parent} {
        // Grid layout
        layout_->setContentsMargins(0, 0, 0, 0);
        layout_->setSpacing(0);
        layout_->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        setLayout(layout_);
    }

    void addCell(Cell* cell, int row, int column) {
        layout_->addWidget(cell, row, column);
        cells_.push_back(cell);
    }

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
        if (Tool::toolType() == Tool::ToolType::kNone) {
            selection_->setGeometry(
                QRect(selection_begin_, e->pos()).normalized());
        }
    }
    void mouseReleaseEvent(QMouseEvent* /*e*/) override {
        if (Tool::toolType() == Tool::ToolType::kNone) {
            selection_->hide();

            unselectActiveCells();
            active_cells_ = IntersectsCells(selection_->geometry());

            for (auto* i : active_cells_) {
                i->setSelected();
            }
        }
    }

    QList<Cell*> IntersectsCells(QRect rect) {
        QList<Cell*> intersects{};

        for (Cell* i : cells_) {
            if (rect.intersects(i->geometry())) {
                intersects.push_back(i);
            }
        }

        return intersects;
    }

    void unselectActiveCells() {
        for (auto* i : active_cells_) {
            i->setSelected(false);
        }

        active_cells_.clear();
    }

   private:
    int scale_{1};
    QGridLayout* layout_{new QGridLayout{}};
    QList<Cell*> cells_;
    QList<Cell*> active_cells_;

    QRubberBand* selection_{new QRubberBand(QRubberBand::Rectangle, this)};
    QPoint selection_begin_;

    inline static constexpr int kMaxScale{10};
    inline static constexpr int kMinScale{-10};
    inline static constexpr int kScaleStep{1};
};