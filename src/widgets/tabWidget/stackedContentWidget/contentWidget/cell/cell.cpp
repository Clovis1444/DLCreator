#include "cell.h"

#include <qsizepolicy.h>

#include "cellAction.h"
#include "src/history.h"
#include "src/tool.h"

Cell::Cell(QWidget* parent, QPoint pos, int cell_size, QColor background_color)
    : QLabel{parent},
      pos_{pos},
      background_color_{background_color},
      size_{cell_size},
      pixmap_{new QPixmap{size_, size_}} {
    setFixedSize(kCellSize, kCellSize);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    drawCell();

    QObject::connect(this, &Cell::clicked, this, &Cell::onClicked);
}

void Cell::setSelected(bool selected) {
    selected_ = selected;
    drawCell();
}

void Cell::setLayer(const CellLayer* layer, bool track_history) {
    if (layer == nullptr) {
        return;
    }

    CellAction a{this};

    switch (layer->type()) {
        case CellLayer::kBackground:
            if (layer_background_ == layer->name()) {
                return;
            }
            layer_background_ = layer->name();
            break;
        case CellLayer::kLiquid:
            if (layer_liquid_ == layer->name()) {
                return;
            }
            layer_liquid_ = layer->name();
            break;
        case CellLayer::kGaz:
            if (layer_gaz_ == layer->name()) {
                return;
            }
            layer_gaz_ = layer->name();
            break;
        case CellLayer::kEnumLength:
            return;
    }

    drawCell();

    if (track_history) {
        a.registerAction();
        History::History::push(parentWidget(), a);
    }
}
void Cell::setLayer(const CellInfo& i) {
    if (i == info()) {
        return;
    }

    layer_background_ = i.background;
    layer_liquid_ = i.liquid;
    layer_gaz_ = i.gaz;

    drawCell();
}
void Cell::clearLayers(bool track_history) {
    if (layer_liquid_.isEmpty() && layer_gaz_.isEmpty()) {
        return;
    }

    CellAction a{this};

    layer_liquid_.clear();
    layer_gaz_.clear();
    drawCell();

    if (track_history) {
        a.registerAction();
        History::History::push(parentWidget(), a);
    }
}

Cell::CellInfo Cell::info() {
    return CellInfo{layer_background_, layer_liquid_, layer_gaz_};
}

QString Cell::background() const { return layer_background_; }
QString Cell::liquid() const { return layer_liquid_; }
QString Cell::gaz() const { return layer_gaz_; }

QPoint Cell::pos() const { return pos_; }
void Cell::setPos(QPoint pos) { pos_ = pos; }

//
// Slots
//
void Cell::onClicked() {
    switch (Tool::toolType()) {
        case Tool::kBackground:
        case Tool::kLiquid:
        case Tool::kGaz:
            setLayer(Tool::cell_layer());
            break;
        case Tool::kClear:
            clearLayers();
            break;
        case Tool::kNone:
        case Tool::kEnumLength:
            break;
    }
}

// Draws cells depending on its member variables.
void Cell::drawCell() {
    // Background
    const auto* background{
        CellLayer::get(CellLayer::Type::kBackground, layer_background_)};
    if (background == nullptr || background->isEmpty()) {
        if (k_use_default_background_ && k_default_background_ != nullptr)
            *pixmap_ = k_default_background_->scaled(size_, size_);
        else
            pixmap_->fill(background_color_);
    } else {
        *pixmap_ = background->pixmap()->scaled(size_, size_);
    }

    QPainter pntr{pixmap_};

    // Frame
    if (selected_) {
        if (k_active_cell_frame_ != nullptr) {
            pntr.drawPixmap(0, 0, k_active_cell_frame_->scaled(size_, size_));
        }
    } else {
        if (k_cell_frame_ != nullptr) {
            pntr.drawPixmap(0, 0, k_cell_frame_->scaled(size_, size_));
        }
    }

    // Liquid
    const auto* liquid{CellLayer::get(CellLayer::Type::kLiquid, layer_liquid_)};
    if (liquid != nullptr && !liquid->isEmpty()) {
        pntr.drawPixmap(0, 0, liquid->pixmap()->scaled(size_, size_));
    }

    // Gaz
    const auto* gaz{CellLayer::get(CellLayer::Type::kGaz, layer_gaz_)};
    if (gaz != nullptr && !gaz->isEmpty()) {
        pntr.drawPixmap(0, 0, gaz->pixmap()->scaled(size_, size_));
    }

    setPixmap(*pixmap_);
}

void Cell::mousePressEvent(QMouseEvent* e) {
    click_pos_ = e->pos();

    QLabel::mousePressEvent(e);
}
void Cell::mouseReleaseEvent(QMouseEvent* e) {
    if (click_pos_ == e->pos()) {
        emit clicked();
    }

    QLabel::mouseReleaseEvent(e);
}