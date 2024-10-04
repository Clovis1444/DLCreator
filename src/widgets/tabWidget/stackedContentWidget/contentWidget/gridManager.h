#pragma once

#include <qevent.h>
#include <qgraphicsscene.h>
#include <qgraphicsview.h>
#include <qobject.h>
#include <qpoint.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cellItem/cellItem.h"

class GridManager : public QGraphicsView {
    Q_OBJECT

   public:
    explicit GridManager(QWidget* parent = nullptr) : QGraphicsView(parent) {
        auto* sc{new QGraphicsScene{this}};
        setScene(sc);

        fillScene(3, 2, 150);
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

    void wheelEvent(QWheelEvent* e) override {
        constexpr double kScaleFactor{1.2};

        // Get mouse pos scene cords
        QPointF mouse_pos{e->position()};
        QPointF scene_mouse_pos{mapToScene(mouse_pos.toPoint())};

        // Zooming
        if (e->angleDelta().y() < 0) {  // Down scroll, zoom out
            scale(1 / kScaleFactor, 1 / kScaleFactor);
        } else {  // Up scroll, zoom in
            scale(kScaleFactor, kScaleFactor);
        }

        // Scroll view to the same cords
        centerOn(scene_mouse_pos);
    };
};