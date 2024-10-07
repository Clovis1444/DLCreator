#pragma once

#include <qevent.h>
#include <qgraphicsscene.h>
#include <qgraphicsview.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
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

        fillScene(1000, 1000, 50);
    }

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
            scale(1 / kZoomFactor_, 1 / kZoomFactor_);
        } else {  // Up scroll, zoom in
            scale(kZoomFactor_, kZoomFactor_);
        }

        // Scroll view to the same cords
        centerOn(scene_mouse_pos);

        QGraphicsView::wheelEvent(e);
    };

    void mousePressEvent(QMouseEvent* e) override {
        switch (e->button()) {
            // Panning
            case Qt::MiddleButton:
                isPanning_ = true;
                lastPanningPoint_ = e->pos();
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

            horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
                                            delta_x * kPanningFactor_);
            verticalScrollBar()->setValue(verticalScrollBar()->value() +
                                          delta_y * kPanningFactor_);

            lastPanningPoint_ = e->pos();
        }

        QGraphicsView::mouseMoveEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent* e) override {
        switch (e->button()) {
            // Panning
            case Qt::MiddleButton:
                isPanning_ = false;
                lastPanningPoint_ = {};
                break;
            default:
                break;
        }

        QGraphicsView::mouseReleaseEvent(e);
    }

    // How fast zooming works
    double kZoomFactor_{1.2};
    bool isPanning_{};
    QPoint lastPanningPoint_;
    // How fast panning scrolls
    int kPanningFactor_{4};
};