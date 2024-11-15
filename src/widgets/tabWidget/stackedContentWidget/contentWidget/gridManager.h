// gridManager.h
//
// Controls:
// - Panning: Shift + LMB move
// - Extend RubberBand: Ctrl + LMB/RMB move
// - Zoom: Mouse wheel Up/Down

#pragma once

#include <qevent.h>
#include <qgraphicsscene.h>
#include <qgraphicsview.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qrubberband.h>
#include <qscrollbar.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cellItem/cellAction.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cellItem/cellItem.h"

class GridManager : public QGraphicsView {
    Q_OBJECT

   public:
    explicit GridManager(QWidget* parent, int size)
        : QGraphicsView{parent}, rows_{size}, cols_{size} {
        constructorBody(size, size);
    }
    explicit GridManager(QWidget* parent, int rows, int cols)
        : QGraphicsView{parent}, rows_{rows}, cols_{cols} {
        constructorBody(rows, cols);
    }

    // TODO(clovis): add impl for this functions
    // void unselectCells() {}
    // void clearSelectedCells() {}
    // bool hasSelection() { return true; }
    // Returns [rows, cols] pair
    QPair<int, int> gridSize() const { return {rows_, cols_}; }
    // void resizeGrid(/* ExpandDirection d, */ bool expand = true) {}
    // void switchExpandButtons(bool positive) {}
    QList<const CellItem*> cellItems() const {
        QList<const CellItem*> list{};

        for (auto* i : items()) {
            auto* ci{dynamic_cast<CellItem*>(i)};
            if (ci) {
                list.push_back(ci);
            }
        }

        return list;
    }
    QList<const CellItem*> cellItems(
        const QRect& rect,
        Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const {
        QList<const CellItem*> list{};

        for (auto* i : items(rect, mode)) {
            auto* ci{dynamic_cast<CellItem*>(i)};
            if (ci) {
                list.push_back(ci);
            }
        }

        return list;
    }
    QList<CellItem*> cellItemsMut() {
        QList<CellItem*> list{};

        for (auto* i : items()) {
            auto* ci{dynamic_cast<CellItem*>(i)};
            if (ci) {
                list.push_back(ci);
            }
        }

        return list;
    }
    QList<CellItem*> cellItemsMut(
        const QRect& rect,
        Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) {
        QList<CellItem*> list{};

        for (auto* i : items(rect, mode)) {
            auto* ci{dynamic_cast<CellItem*>(i)};
            if (ci) {
                list.push_back(ci);
            }
        }

        return list;
    }

   private slots:
    void onCellItemSetSelection(bool select) {
        auto* ci{dynamic_cast<CellItem*>(sender())};
        if (!ci) {
            return;
        }

        // On unselect
        if (!select) {
            selected_cells_.removeOne(ci);
        }
        // On select and cell is not in the list
        else if (!selected_cells_.contains(ci)) {
            selected_cells_.push_back(ci);
        }
    }

   private:
    void fillScene(int rows, int cols, qreal rect_size = 100) {
        // Note: One-based indexing
        for (int i{1}; i <= rows; ++i) {
            for (int j{1}; j <= cols; ++j) {
                auto* cell{new CellItem{rect_size, rect_size, rect_size * j,
                                        rect_size * i}};

                QObject::connect(cell, &CellItem::selected, this,
                                 [this]() { onCellItemSetSelection(true); });
                QObject::connect(cell, &CellItem::unselected, this,
                                 [this]() { onCellItemSetSelection(false); });

                scene()->addItem(cell);
            }
        }
    }

    void clearSelection() {
        // Important: Must copy selected_cells_ container because
        // CellItem::setSelected() modifies selected_cells_
        QList<CellItem*> list{selected_cells_};

        for (CellItem* i : list) {
            i->setSelected(false);
        }
    }

    // Zooming implementation
    void wheelEvent(QWheelEvent* e) override {
        // Get mouse pos scene cords
        QPointF mouse_pos{e->position()};
        QPointF scene_mouse_pos{mapToScene(mouse_pos.toPoint())};

        // Zooming
        if (e->angleDelta().y() < 0) {  // Down scroll, zoom out
            if (kCurrentZoomStep_ <= kMinZoomStep_) {
                return;
            }
            scale(1 / kZoomFactor_, 1 / kZoomFactor_);
            --kCurrentZoomStep_;
        } else {  // Up scroll, zoom in
            if (kCurrentZoomStep_ >= kMaxZoomStep_) {
                return;
            }
            scale(kZoomFactor_, kZoomFactor_);
            ++kCurrentZoomStep_;
        }

        // Scroll view to the same cords
        centerOn(scene_mouse_pos);

        QGraphicsView::wheelEvent(e);
    };
    // Panning implementation
    void mousePressEvent(QMouseEvent* e) override {
        switch (e->button()) {
            case Qt::LeftButton:
                // Panning
                if (e->modifiers().testFlag(Qt::ShiftModifier)) {
                    isPanning_ = true;
                    lastPanningPoint_ = e->pos();
                    setCursor(Qt::ClosedHandCursor);

                    // Do not propogate event
                    return;
                }
                break;
            // Selection
            case Qt::RightButton:
                isSelecting_ = true;
                curr_selection_buffer_.clear();
                selection_begin_ = e->pos();

                clearSelection();
                // TODO(clovis): create extendSelectionModifierKey in settings.h
                // If shift is pressed - select prev selection
                if (e->modifiers().testFlag(Qt::ShiftModifier)) {
                    isExtendingSelection_ = true;

                    for (CellItem* i : prev_selection_) {
                        i->setSelected(true);
                    }
                }

                selection_->setGeometry(QRect{selection_begin_, QSize{}});
                selection_->show();
                break;
            default:
                break;
        }

        QGraphicsView::mousePressEvent(e);
    }
    void mouseMoveEvent(QMouseEvent* e) override {
        // Panning
        if (isPanning_) {
            int delta_x{e->pos().x() - lastPanningPoint_.x()};
            int delta_y{e->pos().y() - lastPanningPoint_.y()};

            horizontalScrollBar()->setValue(horizontalScrollBar()->value() -
                                            (delta_x * kPanningFactor_));
            verticalScrollBar()->setValue(verticalScrollBar()->value() -
                                          (delta_y * kPanningFactor_));

            lastPanningPoint_ = e->pos();
        }

        // Selection
        if (isSelecting_) {
            selection_->setGeometry(
                QRect(selection_begin_, e->pos()).normalized());

            // Revert CellAction
            for (CellItem* i : curr_selection_buffer_) {
                // TODO(clovis): implement revert action here

                // If extending selection
                if (isExtendingSelection_ && prev_selection_.contains(i)) {
                    continue;
                }

                i->setSelected(false);
            }

            // Iterate moved selection
            for (CellItem* i : cellItemsMut(selection_->geometry())) {
                if (!curr_selection_buffer_.contains(i)) {
                    // TODO(clovis): implement action registration here
                    curr_selection_buffer_.push_back(i);
                }
                i->setSelected(true);
            }
        }

        QGraphicsView::mouseMoveEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent* e) override {
        switch (e->button()) {
            // Panning
            case Qt::LeftButton:
                isPanning_ = false;
                lastPanningPoint_ = {};
                setCursor(Qt::ArrowCursor);
                break;
            // Selection
            case Qt::RightButton: {
                selection_->hide();

                QList<CellItem*> selection{
                    cellItemsMut(selection_->geometry())};

                // update prev_selection_
                if (isExtendingSelection_) {
                    prev_selection_ += selection;
                } else {
                    prev_selection_ = selection;
                }
                // Select
                for (CellItem* i : selection) {
                    i->setSelected(true);
                }

                isSelecting_ = false;
                isExtendingSelection_ = false;
                break;
            }
            default:
                break;
        }

        QGraphicsView::mouseReleaseEvent(e);
    }

    void constructorBody(int rows, int cols, qreal rect_size = 100) {
        if (scene() == nullptr) {
            auto* sc{new QGraphicsScene{this}};
            setScene(sc);
        }

        // do nothing if scene is not empty
        if (!scene()->items().isEmpty()) {
            return;
        }

        fillScene(rows, cols, rect_size);
    }

    int rows_;
    int cols_;

    // How fast zooming works
    double kZoomFactor_{1.2};
    int kMaxZoomStep_{10};
    int kMinZoomStep_{-10};
    int kCurrentZoomStep_{};

    // Panning
    bool isPanning_{};
    QPoint lastPanningPoint_;
    // How fast panning scrolls
    // TODO(clovis): This factor should be 1. Remove this var?
    int kPanningFactor_{1};

    // Selection
    bool isSelecting_{};
    bool isExtendingSelection_{};
    QRubberBand* selection_{new QRubberBand{QRubberBand::Rectangle, this}};
    QPoint selection_begin_;
    QList<CellItem*> prev_selection_;
    // Note: contains ALL CellItems that was touched by current selection
    QList<CellItem*> curr_selection_buffer_;
    QHash<CellItem*, CellAction> selection_buffer_;
    // TODO(clovis): try another approach(without) using signals for
    // selected_cells_
    QList<CellItem*> selected_cells_;

    // TODO(clovis): deallocate memory in destructor
    // TODO(clovis): add keyboard shortcuts
    // TODO(clovis): implement variable rubber band color
};
