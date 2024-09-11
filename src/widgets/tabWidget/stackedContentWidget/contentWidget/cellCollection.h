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
        : QWidget{parent}, cols_{size}, rows_{size} {
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

    QPair<int, int> gridSize() const { return {cols_, rows_}; }

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

    QList<const Cell*> cellList() const {
        QList<const Cell*> list{};

        for (const auto* i : cells_) {
            list.push_back(i);
        }

        return list;
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
        layout_->addWidget(expand_buttons_[kUp], 0, 1, 1, cols_);

        layout_->addWidget(expand_buttons_[kDown], rows_ + 1, 1, 1, cols_);

        layout_->addWidget(expand_buttons_[kLeft], 1, 0, rows_, 1);

        layout_->addWidget(expand_buttons_[kRight], 1, cols_ + 1, rows_, 1);
    }

    // TODO(clovis): refactor this like shrinkGrid?
    void expandGrid(ExpandDirection d) {
        switch (d) {
            case kUp:
                if (rows_ <= 0) {
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
                for (int i{1}; i <= cols_; ++i) {
                    auto* cell{new Cell{this, {i, 1}}};

                    addCell(cell);
                }

                ++rows_;
                break;
            case kDown:
                for (int i{1}; i <= cols_; ++i) {
                    auto* cell{new Cell{this, {i, rows_ + 1}}};

                    addCell(cell);
                }
                ++rows_;
                break;
            case kLeft:
                if (cols_ <= 0) {
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
                for (int j{1}; j <= rows_; ++j) {
                    auto* cell{new Cell{this, {1, j}}};

                    addCell(cell);
                }

                ++cols_;
                break;
            case kRight:
                for (int j{1}; j <= rows_; ++j) {
                    auto* cell{new Cell{this, {cols_ + 1, j}}};

                    addCell(cell);
                }
                ++cols_;
                break;
            case kEnumLength:
                return;
        }

        placeExpandButtons();
    }
    void shrinkGrid(ExpandDirection d) {
        // TODO(clovis): reduce amount of parameters to two bools?
        auto func = [this](int position, bool UpOrLeft, int& size_to_change,
                           bool LeftOrRight) {
            //
            if (size_to_change <= 1) {
                return;
            }

            //
            QList<Cell*> to_remove{};
            for (auto* i : cells_) {
                auto pos{i->pos()};
                //
                if (LeftOrRight) {
                    if (pos.x() == position) {
                        to_remove.push_back(i);
                    }
                    // Update Remaining Cells pos
                    else {
                        //
                        if (UpOrLeft) {
                            pos.setX(pos.x() - 1);
                            i->setPos(pos);
                            layout_->addWidget(i, pos.y(), pos.x());
                        }
                    }
                }
                //
                if (!LeftOrRight) {
                    if (pos.y() == position) {
                        to_remove.push_back(i);
                    }
                    // Update Remaining Cells pos
                    else {
                        //
                        if (UpOrLeft) {
                            pos.setY(pos.y() - 1);
                            i->setPos(pos);
                            layout_->addWidget(i, pos.y(), pos.x());
                        }
                    }
                }
            }
            // Remove cells
            for (auto* i : to_remove) {
                cells_.removeOne(i);
                selected_cells_.removeOne(i);
                delete i;
            }

            //
            --size_to_change;
        };

        switch (d) {
            case kUp: {
                func(1, true, rows_, false);
                break;
            }
            case kDown: {
                func(rows_, false, rows_, false);
                break;
            }
            case kLeft: {
                func(1, true, cols_, true);
                break;
            }
            case kRight: {
                func(cols_, false, cols_, true);
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
    int cols_;
    int rows_;
    QGridLayout* layout_{new QGridLayout{this}};
    // TODO(clovis): change(?) QList to a proper data type. Consider something
    // like QHash<QPoint, Cell*>
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