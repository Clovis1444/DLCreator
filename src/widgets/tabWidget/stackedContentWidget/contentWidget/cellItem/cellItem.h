// cellItem.h

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
#include <qpen.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvectornd.h>
#include <qwidget.h>

// #include <QGraphicsItem>

#include "cellLayer.h"
#include "src/settings.h"

class CellItem : public QObject, public QGraphicsItem {
    Q_OBJECT

   public:
    explicit CellItem(qreal width, qreal height, qreal x = 0, qreal y = 0,
                      QColor background_color =
                          Settings::CellItem::kDefaultCellItemBackgroundColor);
    explicit CellItem(
        QPoint pos, qreal cell_size = Settings::CellItem::kDefaultCellItemSize,
        QColor background_color =
            Settings::CellItem::kDefaultCellItemBackgroundColor);

    struct CellInfo {
        QString background;
        QString liquid;
        QString gaz;
        bool selected;

        bool operator==(const CellInfo& other) const = default;
        // Const
        QString operator[](const CellLayer::Type& type) const {
            switch (type) {
                case CellLayer::kBackground:
                    return background;
                case CellLayer::kLiquid:
                    return liquid;
                case CellLayer::kGaz:
                    return gaz;
                default:
                    return subscript_operator_overflow_buffer_;
            }
        }
        // Mut
        QString& operator[](CellLayer::Type type) {
            switch (type) {
                case CellLayer::kBackground:
                    return background;
                case CellLayer::kLiquid:
                    return liquid;
                case CellLayer::kGaz:
                    return gaz;
                default:
                    return subscript_operator_overflow_buffer_;
            }
        }

       protected:
        inline static QString subscript_operator_overflow_buffer_{};
    };

    // IMPORTANT: function hiding to reimplement selected functionality
    bool isSelected() const;
    void setSelected(bool select);

    QPoint gridPos() const;

    void setLayer(const CellLayer* layer, bool track_history = true);
    void setLayer(const CellInfo& i);
    void clearLayers(bool track_history = true);
    CellInfo info() const;

    QString background() const;
    QString liquid() const;
    QString gaz() const;

    // TODO(clovis): resolve naming conflict with QGrapgicsItem::(pos/setpos)
    // QPoint pos() const;
    // void setPos(QPoint pos);

   signals:

   private:
    // void mousePressEvent(QGraphicsSceneMouseEvent* e) override {
    //     switch (e->button()) {
    //             // If LMB was pressed
    //             qInfo() << "Mouse pos: " << e->scenePos();
    //             qInfo() << "Item pos: " << scenePos();
    //             qInfo() << "Bounding rect: " << boundingRect() << '\n';
    //
    //             break;
    //         default:
    //             break;
    //     }
    //
    //     QGraphicsItem::mousePressEvent(e);
    // }

    void constructorBody(qreal pos_x, qreal pos_y);

    void paint(QPainter* pntr, const QStyleOptionGraphicsItem* /*option*/,
               QWidget* /*widget*/) override;

    QRectF boundingRect() const override;
    QRectF frameRect(const QPen& frame_pen) const;

    int width() const;
    int height() const;

    void updateToolTip();

    qreal width_;
    qreal height_;

    bool selected_{};

    QColor background_color_;
    QString layer_background_{"Dirt"};
    QString layer_liquid_{""};
    QString layer_gaz_{""};
};
