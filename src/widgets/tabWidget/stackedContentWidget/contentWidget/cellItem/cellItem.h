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

#include "src/settings.h"
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellLayer.h"

class CellItem : public QGraphicsItem {
   public:
    explicit CellItem(qreal width, qreal height, qreal x = 0, qreal y = 0,
                      QColor background_color =
                          Settings::CellItem::kDefaultCellItemBackgroundColor)
        : width_{width},
          height_{height},
          background_color_{background_color},
          kC_{++count_} {
        setPos(x, y);
    }

    explicit CellItem(
        QPoint pos, qreal cell_size = Settings::CellItem::kDefaultCellItemSize,
        QColor background_color =
            Settings::CellItem::kDefaultCellItemBackgroundColor)
        : width_{cell_size},
          height_{cell_size},
          background_color_{background_color},
          kC_{++count_} {
        setPos(pos.x(), pos.y());
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
                break;
        }

        QGraphicsItem::mousePressEvent(e);
    }

    QRectF boundingRect() const override {
        return QRectF{0, 0, width_, height_};
    }

    void paint(QPainter* pntr, const QStyleOptionGraphicsItem* /*option*/,
               QWidget* /*widget*/) override {
        using CellItem = Settings::CellItem;

        //
        // Background
        //
        const auto* background{
            CellLayer::get(CellLayer::Type::kBackground, layer_background_)};
        const auto* default_background{
            CellLayer::get(CellLayer::Type::kBackground,
                           CellItem::kDefaultBackgroundLayerName)};
        // If current background is invalid
        if (background == nullptr || background->isEmpty()) {
            // If UseDefaultBackground and default background is valid
            if (CellItem::kUseDefaultBackgroundLayer &&
                default_background != nullptr &&
                !default_background->isEmpty()) {
                // Draw default background
                pntr->drawPixmap(
                    0, 0,
                    default_background->pixmap()->scaled(width(), height()));
            }
            // If default background is invalid
            else {
                // Fill background with default color
                pntr->fillRect(boundingRect(),
                               CellItem::kDefaultCellItemBackgroundColor);
            }
        }
        // If current background is valid
        else {
            // Draw current background
            pntr->drawPixmap(0, 0,
                             background->pixmap()->scaled(width(), height()));
        }
        // TODO(clovis): refactor cellframe: put it in CellLayer?
        //
        // Draw frame
        //
        pntr->drawRect(boundingRect());

        //
        // Liquid
        //
        const auto* liquid{
            CellLayer::get(CellLayer::Type::kLiquid, layer_liquid_)};
        if (liquid != nullptr && !liquid->isEmpty()) {
            pntr->drawPixmap(0, 0, liquid->pixmap()->scaled(width(), height()));
        }

        //
        // Gaz
        //
        const auto* gaz{CellLayer::get(CellLayer::Type::kGaz, layer_gaz_)};
        if (gaz != nullptr && !gaz->isEmpty()) {
            pntr->drawPixmap(0, 0, gaz->pixmap()->scaled(width(), height()));
        }

        pntr->drawText(0, static_cast<int>(height_ / 2), QString::number(kC_));
    }

    int width() const { return static_cast<int>(width_); }
    int height() const { return static_cast<int>(height_); }

    qreal width_;
    qreal height_;
    QColor background_color_;
    QString layer_background_;
    QString layer_liquid_{"Water"};
    QString layer_gaz_{"Smoke"};
    bool selected_{false};

    // TODO(clovis): remove this counter
    const int kC_;
    inline static int count_{0};
};
