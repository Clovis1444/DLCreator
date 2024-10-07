#pragma once

#include <qbrush.h>
#include <qcolor.h>
#include <qevent.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qtypes.h>
#include <qvectornd.h>
#include <qwidget.h>

class CellItem : public QGraphicsItem {
   public:
    explicit CellItem(qreal width, qreal height, qreal x = 0, qreal y = 0,
                      QGraphicsItem* parent = nullptr)
        : QGraphicsItem{parent}, width_{width}, height_{height}, kC_{++count_} {
        setPos(x, y);
    }

   private:
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override {
        switch (e->button()) {
                // If LMB was pressed
            case Qt::LeftButton:
                qInfo() << "Mouse pos: " << e->scenePos();
                qInfo() << "Item pos: " << scenePos();
                qInfo() << "Bounding rect: " << boundingRect();
                break;
            default:
        }

        QGraphicsItem::mousePressEvent(e);
    }

    QRectF boundingRect() const override {
        return QRectF{0, 0, width_, height_};
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/,
               QWidget* /*widget*/) override {
        // Fill background
        painter->fillRect(boundingRect(), Qt::red);
        // Draw frame
        painter->drawRect(boundingRect());

        painter->drawText(0, static_cast<int>(height_ / 2),
                          QString::number(kC_));
    }

    qreal width_;
    qreal height_;

    // TODO(clovis): remove this counter
    const int kC_;
    inline static int count_{0};
};