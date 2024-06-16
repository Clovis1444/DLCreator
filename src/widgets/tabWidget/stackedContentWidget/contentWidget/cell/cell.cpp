#include "cell.h"

#include "../../../../../history.h"
#include "../../../../../tool.h"
#include "cellAction.h"

Cell::Cell(QWidget* parent, int cell_size, QColor background_color)
    : QLabel{parent},
      background_color_{background_color},
      size_{cell_size},
      pixmap_{new QPixmap{size_, size_}} {
    setFixedSize(kCellSize, kCellSize);
    drawCell();

    QObject::connect(this, &Cell::clicked, this, &Cell::onClicked);
}

void Cell::setSelected(bool selected) {
    selected_ = selected;
    drawCell();
}

void Cell::setLayer(const Liquid* liquid, bool track_history) {
    if (liquid->name() == liquid_) {
        return;
    }

    CellAction a{this};

    if (liquid) {
        liquid_ = liquid->name();
        drawCell();
    }

    if (track_history) {
        a.registerAction();
        History::History::push(a);
    }
}
void Cell::setLayer(const Gaz* gaz, bool track_history) {
    if (gaz->name() == gaz_) {
        return;
    }

    CellAction a{this};

    if (gaz) {
        gaz_ = gaz->name();
        drawCell();
    }

    if (track_history) {
        a.registerAction();
        History::History::push(a);
    }
}
void Cell::setLayer(const Background* background, bool track_history) {
    if (background->name() == background_) {
        return;
    }

    CellAction a{this};

    if (background) {
        background_ = background->name();
        drawCell();
    }

    if (track_history) {
        a.registerAction();
        History::History::push(a);
    }
}
void Cell::setLayer(const CellInfo& i) {
    if (i == info()) {
        return;
    }

    background_ = i.background;
    liquid_ = i.liquid;
    gaz_ = i.gaz;

    drawCell();
}
void Cell::clearLayers(bool track_history) {
    if (liquid_.isEmpty() && gaz_.isEmpty()) {
        return;
    }

    CellAction a{this};

    liquid_.clear();
    gaz_.clear();
    drawCell();

    if (track_history) {
        a.registerAction();
        History::History::push(a);
    }
}

Cell::CellInfo Cell::info() { return CellInfo{background_, liquid_, gaz_}; }

QString Cell::background() const { return background_; }
QString Cell::liquid() const { return liquid_; }
QString Cell::gaz() const { return gaz_; }

//
// Slots
//
void Cell::onClicked() {
    switch (Tool::toolType()) {
        case Tool::kLiquid:
            setLayer(static_cast<const Liquid*>(Tool::cell_layer()));
            break;
        case Tool::kGaz:
            setLayer(static_cast<const Gaz*>(Tool::cell_layer()));
            break;
        case Tool::kClear:
            clearLayers();
            break;
        case Tool::kBackground:
            setLayer(static_cast<const Background*>(Tool::cell_layer()));
            break;
        case Tool::kNone:
        case Tool::kEnumLength:
            break;
    }
}

// Draws cells depending on its member variables.
void Cell::drawCell() {
    // Background
    const auto* background{Background::get(background_)};
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
    const auto* liquid{Liquid::get(liquid_)};
    if (liquid != nullptr && !liquid->isEmpty()) {
        pntr.drawPixmap(0, 0, liquid->pixmap()->scaled(size_, size_));
    }

    // Gaz
    const auto* gaz{Gaz::get(gaz_)};
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