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

#include "src/settings.h"
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
    // rect is in Viewport coordinates
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
    // rect is in Viewport coordinates
    QList<CellItem*> cellItemsMut(
        const QRect& rect,
        Qt::ItemSelectionMode mode = Qt::IntersectsItemBoundingRect) {
        QList<CellItem*> list{};

        for (auto* i : items(rect, mode)) {
            auto* ci{dynamic_cast<CellItem*>(i)};
            if (ci) {
                list.push_back(ci);
            }
        }

        return list;
    }

   private:
    void fillScene(int rows, int cols, qreal rect_size = 100) {
        // Note: One-based indexing
        for (int i{1}; i <= rows; ++i) {
            for (int j{1}; j <= cols; ++j) {
                auto* cell{new CellItem{rect_size, rect_size, rect_size * j,
                                        rect_size * i}};

                scene()->addItem(cell);
            }
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
                // Panning
            case Qt::MiddleButton:
            case Qt::LeftButton:
                panningPress(e);

                // Do not propogate event
                return;
                break;
            // Selection
            case Qt::RightButton:
                selectionPress(e);
                break;
            default:
                break;
        }

        QGraphicsView::mousePressEvent(e);
    }
    void mouseMoveEvent(QMouseEvent* e) override {
        // Panning
        if (isPanning_) {
            panningMove(e);
        }

        // Selection
        if (isSelecting_) {
            selectionMove(e);
        }

        QGraphicsView::mouseMoveEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent* e) override {
        switch (e->button()) {
            // Panning
            case Qt::LeftButton:
            case Qt::MiddleButton:
                panningRelease();
                break;
            // Selection
            case Qt::RightButton: {
                selectionRelease();
                break;
            }
            default:
                break;
        }

        QGraphicsView::mouseReleaseEvent(e);
    }

    // Panning implementation
    void panningPress(QMouseEvent* e) {
        bool mouse_pan{Settings::GridManager::kUseWheelPanning &&
                       e->button() == Qt::MiddleButton};
        bool keyboard_pan{Settings::GridManager::kUseKeyboardPanning &&
                          e->button() == Qt::LeftButton &&
                          e->modifiers().testFlag(
                              Settings::GridManager::kPanningModifierKey)};

        if (mouse_pan || keyboard_pan) {
            isPanning_ = true;
            lastPanningPoint_ = e->pos();
            setCursor(Qt::ClosedHandCursor);
        }
    }
    void panningMove(QMouseEvent* e) {
        int delta_x{e->pos().x() - lastPanningPoint_.x()};
        int delta_y{e->pos().y() - lastPanningPoint_.y()};

        horizontalScrollBar()->setValue(horizontalScrollBar()->value() -
                                        delta_x);
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta_y);

        lastPanningPoint_ = e->pos();
    }
    void panningRelease() {
        isPanning_ = false;
        lastPanningPoint_ = {};
        setCursor(Qt::ArrowCursor);
    }

    // Selection implementation
    void selectionPress(QMouseEvent* e) {
        isSelecting_ = true;
        curr_selection_buffer_.clear();
        selection_begin_ = e->pos();
        // All items in the scene MUST be CellItem or this line will crash
        // selection_target_ = dynamic_cast<CellItem*>(itemAt(e->pos()));

        clearSelection();
        // If shift is pressed - select prev selection
        if (e->modifiers().testFlag(
                Settings::GridManager::kExtendSelectionModifierKey)) {
            isExtendingSelection_ = true;

            setCellsSelected(prev_selection_, true);
        }

        selection_->setGeometry(QRect{selection_begin_, QSize{}});
        selection_->show();
    }
    // TODO(clovis): fix left-based selection
    void selectionMove(QMouseEvent* e) {
        selection_->setGeometry(QRect(selection_begin_, e->pos()).normalized());

        // Early return if selection target didnt change
        if (selection_target_ == itemAt(e->pos())) {
            skip_frame_ = true;
            return;
        }

        updateSelectionTarget(e->pos());

        // if (skip_frame_) {
        //     skip_frame_ = !skip_frame_;
        //
        //     return;
        // }

        // All items in the scene MUST be CellItem or this line will crash
        selection_target_ = dynamic_cast<CellItem*>(itemAt(e->pos()));
        if (!selection_target_) {
            return;
        }

        QList<CellItem*> cells_in_selection{
            cellItemsMut(selection_->geometry())};

        // Update selected_cells_
        // setCellsSelected(cells_in_selection, true);

        // Revert CellAction
        for (CellItem* i : curr_selection_buffer_) {
            // TODO(clovis): implement revert action here

            // If extending selection
            if (isExtendingSelection_ && prev_selection_.contains(i)) {
                continue;
            }

            setCellsSelected(i, false);
        }

        // Iterate moved selection
        for (CellItem* i : cells_in_selection) {
            if (!curr_selection_buffer_.contains(i)) {
                // TODO(clovis): implement action registration here
                curr_selection_buffer_.push_back(i);
            }
            setCellsSelected(i, true);
        }
    }
    void selectionRelease() {
        selection_->hide();

        QList<CellItem*> selection{cellItemsMut(selection_->geometry())};

        // update prev_selection_
        if (isExtendingSelection_) {
            prev_selection_ += selection;
        } else {
            prev_selection_ = selection;
        }
        // Select
        // setCellsSelected(selection, true);

        isSelecting_ = false;
        isExtendingSelection_ = false;
        selection_target_ = nullptr;
    }

    void setCellsSelected(const QList<CellItem*>& cl, bool select) {
        for (CellItem* i : cl) {
            if (select && !selected_cells_.contains(i)) {
                selected_cells_.push_back(i);
            }
            if (!select) {
                selected_cells_.removeOne(i);
            }

            i->setSelected(select);
        }
    }
    void setCellsSelected(CellItem* ci, bool select) {
        if (select && !selected_cells_.contains(ci)) {
            selected_cells_.push_back(ci);
        }
        if (!select) {
            selected_cells_.removeOne(ci);
        }

        ci->setSelected(select);
    }

    void clearSelection() {
        for (CellItem* i : selected_cells_) {
            i->setSelected(false);
        }

        selected_cells_.clear();
    }

    // Call this function inside mouseMoveEvent to update selection_target_
    // state. Returns true if selection_target_ was changed.
    bool updateSelectionTarget(const QPoint& target_pos) {
        CellItem* new_target{dynamic_cast<CellItem*>(itemAt(target_pos))};

        if (!new_target || !selection_target_) {
            return false;
        }

        QList<CellItem*> cells_in_selection{
            cellItemsMut(selection_->geometry())};

        // qInfo() << "Items: " << items(selection_->geometry()).count();
        //
        // qInfo() << "Geometry: " << selection_->geometry()
        //         << "| Cells inside: " << cells_in_selection.count();
        //
        qInfo() << "Event pos: " << mapToScene(target_pos);
        // if (selection_target_ != nullptr) {
        //     qInfo() << "Before: " << selection_target_->sceneBoundingRect();
        // }
        //
        // qInfo() << "After: " << new_target->sceneBoundingRect() << '\n';

        QRectF otr{selection_target_->sceneBoundingRect()};
        QRectF ntr{new_target->sceneBoundingRect()};

        // qInfo() << "SELECTION: " << QRect(selection_begin_, target_pos);
        qInfo() << "Selection_begin: " << selection_begin_;

        Qt::ItemSelectionMode sm{};

        bool move_to_rigth{
            (otr.x() > ntr.x() && target_pos.x() > selection_begin_.y())};
        bool move_to_down{
            (otr.y() > ntr.y() && target_pos.y() > selection_begin_.y())};
        if (move_to_rigth || move_to_down) {
            qInfo() << "Shape";
            sm = Qt::IntersectsItemShape;
        } else {
            sm = Qt::IntersectsItemBoundingRect;
        }

        qInfo() << cellItemsMut(selection_->geometry(), sm).count();

        return true;
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
    // Current item at what selection is
    CellItem* selection_target_{};
    bool skip_frame_{true};

    // TODO(clovis): deallocate memory in destructor
    // TODO(clovis): add keyboard shortcuts
    // TODO(clovis): implement variable rubber band color
};
