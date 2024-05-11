#pragma once

#include <qbrush.h>
#include <qcolor.h>
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qgraphicsview.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpen.h>
#include <qpoint.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include <QMouseEvent>

#include "graphicsscene.h"

class ContentView : public QGraphicsView {
    Q_OBJECT

   public:
    explicit ContentView(QWidget* parent, int size = kDefaultContentViewSize,
                         int cell_size = kCellSize)
        : QGraphicsView{parent},
          scene_{new GraphicsScene{this, size, cell_size}} {
        setScene(scene_);

        // QObject::connect(scene_, &GraphicsScene::mousePressed, this,
        //                  &ContentView::onSceneMousePress);
    }

   public slots:
    void onSceneMousePress() { qDebug() << scene_->lastMousePressPos(); }

   protected:
    // QRectF get_cell_rect(int x, int y) {
    //     int grid_width{grid_pen_->width()};
    //     if ((x % (cell_size_ + grid_width) <= grid_width &&
    //          x % (cell_size_ + grid_width) != 0) ||
    //         (y % (cell_size_ + grid_width) <= grid_width &&
    //          y % (cell_size_ + grid_width) != 0)) {
    //         // grid cord
    //     }
    //     // int top_left{x / (cell_size_ + grid_width)};
    //     // int bottom_left{y / (cell_size_ + grid_width)};

    //     return QRectF{0, 0, 0, 0};
    // }

    GraphicsScene* scene_{};
};
