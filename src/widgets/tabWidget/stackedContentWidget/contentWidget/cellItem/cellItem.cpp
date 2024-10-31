// cellItem.cpp

#include "cellItem.h"

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////PUBLIC////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CellItem::CellItem(qreal width, qreal height, qreal x, qreal y,
                   QColor background_color)
    : width_{width}, height_{height}, background_color_{background_color} {
    constructorBody(x, y);
}
CellItem::CellItem(QPoint pos, qreal cell_size, QColor background_color)
    : width_{cell_size},
      height_{cell_size},
      background_color_{background_color} {
    constructorBody(pos.x(), pos.y());
}

// void setLayer(const CellLayer* layer, bool track_history = true);
// void setLayer(const CellInfo& i);
// void clearLayers(bool track_history = true);
// CellInfo info() const;
//
// QString background() const;
// QString liquid() const;
// QString gaz() const;

// TODO(clovis): resolve naming conflict with QGrapgicsItem::(pos/setpos)
// QPoint pos() const;
// void setPos(QPoint pos);

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////PRIVATE////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void CellItem::constructorBody(qreal pos_x, qreal pos_y) {
    setPos(pos_x, pos_y);

    // CellItem should always be selectable
    setFlags(ItemIsSelectable);
};

void CellItem::paint(QPainter* pntr, const QStyleOptionGraphicsItem* /*option*/,
                     QWidget* /*widget*/) {
    using CellItem = Settings::CellItem;

    //
    // Background
    //
    const auto* background{
        CellLayer::get(CellLayer::Type::kBackground, layer_background_)};
    const auto* default_background{CellLayer::get(
        CellLayer::Type::kBackground, CellItem::kDefaultBackgroundLayerName)};
    // If current background is invalid
    if (background == nullptr || background->isEmpty()) {
        // If UseDefaultBackground and default background is valid
        if (CellItem::kUseDefaultBackgroundLayer &&
            default_background != nullptr && !default_background->isEmpty()) {
            // Draw default background
            pntr->drawPixmap(
                0, 0, default_background->pixmap()->scaled(width(), height()));
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
        pntr->drawPixmap(0, 0, background->pixmap()->scaled(width(), height()));
    }

    //
    // Liquid
    //
    const auto* liquid{CellLayer::get(CellLayer::Type::kLiquid, layer_liquid_)};
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

    //
    // Draw frame
    //
    pntr->setPen(CellItem::framePen(isSelected()));
    pntr->drawRect(frameRect(pntr->pen()));
}

QRectF CellItem::boundingRect() const { return QRectF{0, 0, width_, height_}; }
QRectF CellItem::frameRect(const QPen& frame_pen) const {
    QRectF br{boundingRect()};
    qreal margin{static_cast<qreal>(frame_pen.width()) / 2.0};
    return br.adjusted(margin, margin, -margin, -margin);
}

int CellItem::width() const { return static_cast<int>(width_); }
int CellItem::height() const { return static_cast<int>(height_); }
