#pragma once

#include <qevent.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qlogging.h>
#include <qtypes.h>
#include <qvectornd.h>
#include <qwidget.h>

class CellItem : public QGraphicsRectItem {
   public:
    explicit CellItem(qreal width, qreal height, qreal x = 0, qreal y = 0,
                      QGraphicsItem* parent = nullptr)
        : QGraphicsRectItem{0, 0, width, height, parent} {
        setPos(x, y);
    }

   private:
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override {
        qInfo() << "Mouse pos: " << e->scenePos();
        qInfo() << "Item pos: " << scenePos();
    }
};