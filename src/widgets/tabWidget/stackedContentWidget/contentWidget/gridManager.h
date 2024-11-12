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
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qrubberband.h>
#include <qscrollbar.h>
#include <qtmetamacros.h>
#include <qwidget.h>

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
    QList<const CellItem*> cellList() const {
        QList<const CellItem*> list{};

        for (auto* i : items()) {
            auto* ci{dynamic_cast<CellItem*>(i)};
            if (ci) {
                list.push_back(ci);
            }
        }

        return list;
    }
    QList<CellItem*> cellListMut() {
        QList<CellItem*> list{};

        for (auto* i : items()) {
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

        // Enable RubberBandDrag by default
        setDragMode(RubberBandDrag);
    }

    int rows_;
    int cols_;

    // How fast zooming works
    double kZoomFactor_{1.2};
    int kMaxZoomStep_{10};
    int kMinZoomStep_{-10};
    int kCurrentZoomStep_{};

    bool isPanning_{};
    QPoint lastPanningPoint_;
    // How fast panning scrolls
    // TODO(clovis): This factor should be 1. Remove this var?
    int kPanningFactor_{1};

    // TODO(clovis): deallocate memory in destructor
    // TODO(clovis): add keyboard shortcuts
    // TODO(clovis): implement variable rubber band color
};
