#pragma once

#include <qapplication.h>
#include <qcontainerfwd.h>
#include <qevent.h>
#include <qgraphicsview.h>
#include <qgridlayout.h>
#include <qhash.h>
#include <qlayout.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qrubberband.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qwidget.h>

#include <QWheelEvent>

#include "../../../../history.h"
#include "../../../../tool.h"
#include "cell/cell.h"
#include "cell/cellAction.h"

class CellCollection : public QWidget {
    Q_OBJECT

   public:
    CellCollection(QWidget* parent, unsigned int size) : QWidget{parent} {
        // Grid layout
        layout_->setContentsMargins(0, 0, 0, 0);
        layout_->setSpacing(0);
        layout_->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        setLayout(layout_);

        for (int i{}; i < size; ++i) {
            for (int j{}; j < size; ++j) {
                auto* cell = new Cell{this};
                addCell(cell, i, j);

                QObject::connect(cell, &Cell::clicked, this,
                                 [this, cell] { onCellClicked(cell); });
            }
        }

        QObject::connect(this, &CellCollection::mouseMoved, this,
                         &CellCollection::onMouseMove);
    }

    void unselectCells() {
        for (auto* i : selected_cells_) {
            i->setSelected(false);
        }

        selected_cells_.clear();
    }
    void clearSelectedCells() {
        for (auto* i : selected_cells_) {
            i->clearLayers();
        }
    }

    bool hasSelection() { return !selected_cells_.isEmpty(); }

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
        selection_begin_ = e->pos();

        selection_->setGeometry(QRect(selection_begin_, QSize()));
        selection_->show();

        QWidget::mousePressEvent(e);
    }
    void mouseMoveEvent(QMouseEvent* e) override {
        selection_->setGeometry(QRect(selection_begin_, e->pos()).normalized());

        unselectCells();
        selected_cells_ = intersectsCells(selection_->geometry());

        for (auto* i : selected_cells_) {
            i->setSelected();
        }

        emit mouseMoved();

        QWidget::mouseMoveEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent* e) override {
        selection_->hide();

        if (selection_begin_ != e->pos()) {
            // Create history action
            pushToHistory();
        }

        changed_cells_.clear();

        QWidget::mouseReleaseEvent(e);
    }

    QList<Cell*> intersectsCells(QRect rect) {
        QList<Cell*> intersects{};

        for (Cell* i : cells_) {
            if (rect.intersects(i->geometry())) {
                intersects.push_back(i);
            }
        }

        return intersects;
    }

    void addCell(Cell* cell, int row, int column) {
        layout_->addWidget(cell, row, column);
        cells_.push_back(cell);
    }

    // This function MUST be called only in mouseReleaseEvent and before
    void pushToHistory() {
        QList<CellAction> list{};
        for (auto* i : selected_cells_) {
            list.append(changed_cells_.value(i));
        }
        auto action{CellAction{list}};

        if (action.changed()) {
            History::History::push(action);
        }
    }

   signals:
    void mouseMoved();
    void cellClicked();

   public slots:
    void onCellClicked(Cell* cell) {
        // TODO(clovis): implement CellPos here
        // qDebug() << layout_->indexOf(cell);
        emit cellClicked();
    }

   protected slots:
    void onMouseMove() {
        if (Tool::toolType() == Tool::kNone ||
            Tool::toolType() == Tool::kEnumLength) {
            return;
        }

        for (auto i : changed_cells_) {
            i.undo();
        }

        for (auto* i : selected_cells_) {
            CellAction action{i};

            switch (Tool::toolType()) {
                case Tool::kBackground:
                case Tool::kLiquid:
                case Tool::kGaz:
                    i->setLayer(Tool::cell_layer(), false);
                    break;
                case Tool::kClear:
                    i->clearLayers(false);
                    break;
                case Tool::kNone:
                case Tool::kEnumLength:
                    break;
            }

            action.registerAction();
            if (!changed_cells_.contains(i)) {
                changed_cells_.insert(i, action);
            }
        }
    }

   private:
    int scale_{1};
    QGridLayout* layout_{new QGridLayout{this}};
    QList<Cell*> cells_;
    QList<Cell*> selected_cells_;

    QRubberBand* selection_{new QRubberBand(QRubberBand::Rectangle, this)};
    QPoint selection_begin_;
    QHash<Cell*, CellAction> changed_cells_;

    inline static constexpr int kMaxScale{10};
    inline static constexpr int kMinScale{-10};
    inline static constexpr int kScaleStep{1};
};