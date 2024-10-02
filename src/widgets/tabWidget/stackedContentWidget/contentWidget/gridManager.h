#pragma once

#include <qevent.h>
#include <qgraphicsscene.h>
#include <qgraphicsview.h>
#include <qobject.h>
#include <qpoint.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class GridManager : public QGraphicsView {
    Q_OBJECT

   public:
    explicit GridManager(QWidget* parent = nullptr) : QGraphicsView(parent) {
        auto* sc{new QGraphicsScene{this}};
        setScene(sc);
        scene()->addRect(1000, 1000, 500, 500);
        scene()->addRect(100, 100, 50, 50);
    }

   protected:
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