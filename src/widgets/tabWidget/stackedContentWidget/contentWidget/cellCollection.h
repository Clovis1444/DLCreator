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
#include <qsize.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvarlengtharray.h>
#include <qwidget.h>

#include <QWheelEvent>

#include "cell/cell.h"
#include "cell/cellAction.h"
#include "src/history.h"
#include "src/tool.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/expandButton.h"

class CellCollection : public QWidget {
    Q_OBJECT

   protected:
    enum ExpandDirection { kUp, kDown, kLeft, kRight, kEnumLength };

   public:
    CellCollection(QWidget* parent, int size)
        : QWidget{parent}, size_hor_{size}, size_ver_{size} {
        // Grid layout
        layout_->setContentsMargins(0, 0, 0, 0);
        layout_->setSpacing(0);
        layout_->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        setLayout(layout_);

        // Expand buttons
        placeExpandButtons();
        QObject::connect(expand_buttons_[kUp], &ExpandButton::pressed, this,
                         &CellCollection::onExpandPress);
        QObject::connect(expand_buttons_[kDown], &ExpandButton::pressed, this,
                         &CellCollection::onExpandPress);
        QObject::connect(expand_buttons_[kLeft], &ExpandButton::pressed, this,
                         &CellCollection::onExpandPress);
        QObject::connect(expand_buttons_[kRight], &ExpandButton::pressed, this,
                         &CellCollection::onExpandPress);

        // Cells
        for (int i{1}; i <= size; ++i) {
            for (int j{1}; j <= size; ++j) {
                auto* cell = new Cell{this, {j, i}};
                addCell(cell);

                QObject::connect(cell, &Cell::clicked, this,
                                 [this, cell] { onCellClicked(cell); });
            }
        }

        // Connects
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

    QPair<int, int> gridSize() { return {size_hor_, size_ver_}; }

    void resizeGrid(ExpandDirection d, bool expand = true) {
        if (expand) {
            expandGrid(d);
        } else {
            shrinkGrid(d);
        }
    }

    void SwitchExpandButtons(bool positive) {
        for (auto* i : expand_buttons_) {
            i->setPositive(positive);
        }
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

    void addCell(Cell* cell) {
        auto pos{cell->pos()};
        layout_->addWidget(cell, pos.y(), pos.x());
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
            History::History::push(this, action);
        }
    }

    void placeExpandButtons() {
        layout_->addWidget(expand_buttons_[kUp], 0, 1, 1, size_hor_);

        layout_->addWidget(expand_buttons_[kDown], size_ver_ + 1, 1, 1,
                           size_hor_);

        layout_->addWidget(expand_buttons_[kLeft], 1, 0, size_ver_, 1);

        layout_->addWidget(expand_buttons_[kRight], 1, size_hor_ + 1, size_ver_,
                           1);
    }

    void expandGrid(ExpandDirection d) {
        switch (d) {
            case kUp:
                if (size_ver_ <= 0) {
                    return;
                }

                // Replace old cells
                for (auto* i : cells_) {
                    auto pos{i->pos()};
                    pos.setY(pos.y() + 1);

                    layout_->addWidget(i, pos.y(), pos.x());
                    i->setPos(pos);
                }

                // Place new cells
                for (int i{1}; i <= size_hor_; ++i) {
                    auto* cell{new Cell{this, {i, 1}}};

                    addCell(cell);
                }

                ++size_ver_;
                break;
            case kDown:
                for (int i{1}; i <= size_hor_; ++i) {
                    auto* cell{new Cell{this, {i, size_ver_ + 1}}};

                    addCell(cell);
                }
                ++size_ver_;
                break;
            case kLeft:
                if (size_hor_ <= 0) {
                    return;
                }

                // Replace old cells
                for (auto* i : cells_) {
                    auto pos{i->pos()};
                    pos.setX(pos.x() + 1);

                    layout_->addWidget(i, pos.y(), pos.x());
                    i->setPos(pos);
                }

                // Place new cells
                for (int j{1}; j <= size_ver_; ++j) {
                    auto* cell{new Cell{this, {1, j}}};

                    addCell(cell);
                }

                ++size_hor_;
                break;
            case kRight:
                for (int j{1}; j <= size_ver_; ++j) {
                    auto* cell{new Cell{this, {size_hor_ + 1, j}}};

                    addCell(cell);
                }
                ++size_hor_;
                break;
            case kEnumLength:
                return;
        }

        placeExpandButtons();
    }
    void shrinkGrid(ExpandDirection d) {
        switch (d) {
            case kUp: {
                if (size_ver_ <= 1) {
                    return;
                }

                QList<Cell*> to_remove{};
                for (auto* i : cells_) {
                    auto pos{i->pos()};
                    if (pos.y() == 1) {
                        to_remove.push_back(i);
                    }
                    // Update Remaining Cells pos
                    else {
                        pos.setY(pos.y() - 1);
                        i->setPos(pos);
                        layout_->addWidget(i, pos.y(), pos.x());
                    }
                }
                // Remove cells
                for (auto* i : to_remove) {
                    cells_.removeOne(i);
                    selected_cells_.removeOne(i);
                    delete i;
                }

                --size_ver_;
                break;
            }
            case kDown: {
                if (size_ver_ <= 1) {
                    return;
                }

                QList<Cell*> to_remove{};
                for (auto* i : cells_) {
                    auto pos{i->pos()};
                    if (pos.y() == size_ver_) {
                        to_remove.push_back(i);
                    }
                }
                // Remove cells
                for (auto* i : to_remove) {
                    cells_.removeOne(i);
                    selected_cells_.removeOne(i);
                    delete i;
                }

                --size_ver_;
                break;
            }
            case kLeft: {
                if (size_hor_ <= 1) {
                    return;
                }

                QList<Cell*> to_remove{};
                for (auto* i : cells_) {
                    auto pos{i->pos()};
                    if (pos.x() == 1) {
                        to_remove.push_back(i);
                    }
                    // Update Remaining Cells pos
                    else {
                        pos.setX(pos.x() - 1);
                        i->setPos(pos);
                        layout_->addWidget(i, pos.y(), pos.x());
                    }
                }
                // Remove cells
                for (auto* i : to_remove) {
                    cells_.removeOne(i);
                    selected_cells_.removeOne(i);
                    delete i;
                }

                --size_hor_;
                break;
            }
            case kRight: {
                if (size_hor_ <= 1) {
                    return;
                }

                QList<Cell*> to_remove{};
                for (auto* i : cells_) {
                    auto pos{i->pos()};
                    if (pos.x() == size_hor_) {
                        to_remove.push_back(i);
                    }
                }
                // Remove cells
                for (auto* i : to_remove) {
                    cells_.removeOne(i);
                    selected_cells_.removeOne(i);
                    delete i;
                }

                --size_hor_;
                break;
            }
            case kEnumLength:
                return;
        }

        placeExpandButtons();
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

    void onExpandPress() {
        auto direction{
            static_cast<ExpandDirection>(expand_buttons_.indexOf(sender()))};

        if (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)) {
            resizeGrid(direction, false);
        } else {
            resizeGrid(direction, true);
        }
    }

   private:
    int scale_{1};
    int size_hor_;
    int size_ver_;
    QGridLayout* layout_{new QGridLayout{this}};
    // TODO(clovis): change QList to a proper data type. Consider something like
    // QHash<QPoint, Cell*>
    QList<Cell*> cells_;
    QList<Cell*> selected_cells_;

    const QVarLengthArray<ExpandButton*, ExpandDirection::kEnumLength>
        expand_buttons_{
            new ExpandButton{this, true}, new ExpandButton{this, true},
            new ExpandButton{this, false}, new ExpandButton{this, false}};

    QRubberBand* selection_{new QRubberBand(QRubberBand::Rectangle, this)};
    QPoint selection_begin_;
    QHash<Cell*, CellAction> changed_cells_;

    inline static constexpr int kMaxScale{10};
    inline static constexpr int kMinScale{-10};
    inline static constexpr int kScaleStep{1};
};