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
    explicit GridManager(QWidget* parent = nullptr) : QGraphicsView(parent) {
        auto* sc{new QGraphicsScene{this}};
        setScene(sc);

        fillScene(100, 100, 50);

        // Enable RubberBandDrag by default
        setDragMode(RubberBandDrag);
    }

    // TODO(clovis): add impl for this functions
    // void unselectCells() {}
    // void clearSelectedCells() {}
    // bool hasSelection() { return true; }
    // // Returns [rows, cols] pair
    // QPair<int, int> gridSize() const { return {}; }
    // void resizeGrid(/* ExpandDirection d, */ bool expand = true) {}
    // void switchExpandButtons(bool positive) {}
    // QList<const Cell*> cellList() const {}
    // QList<Cell*> cellListMut() {}

   private:
    void fillScene(int rows, int cols, qreal rect_size = 100) {
        for (int i{}; i < rows; ++i) {
            for (int j{}; j < cols; ++j) {
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
