#pragma once

#include <qabstractscrollarea.h>
#include <qboxlayout.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>
#include <qwidget.h>

// TODO(clovis): create default background Cell color to fill
inline constexpr int kCellSize{50};
inline constexpr int kDefaultContentWidgetSize{10};

// TODO(clovis): create functions that return associated QPixmap?
enum class Liquid { kNone, kWater, kOil, kEnumLegth };
enum class Gaz { kNone, kSmoke, kPoison, kEnumLength };

class Cell : public QLabel {
    Q_OBJECT

   public:
    explicit Cell(QWidget* parent, int cell_size = kCellSize)
        : QLabel{parent}, size_{cell_size}, pixmap_{new QPixmap{size_, size_}} {
        setFixedSize(kCellSize, kCellSize);
        drawCell();

        QObject::connect(this, &Cell::clicked, this, &Cell::onClicked);
    }

    void setLiquid(Liquid liquid) { liquid_ = liquid; }
    void setGaz(Gaz gaz) { gaz_ = gaz; }

    // TODO(clovis): change init/create one init function for all classes?
    static void init_resources() {
        active_back_ = new QPixmap{":/active.png"};
        deactive_back_ = new QPixmap{":/deactive.png"};
        water_ = new QPixmap{":/water.png"};
        oil_ = new QPixmap{":/oil.png"};
        smoke_ = new QPixmap{":/smoke.png"};
        poison_ = new QPixmap{":/poison.png"};
    }

   signals:
    void clicked();

   protected slots:
    void onClicked() {
        active_ = !active_;

        setLiquid(Liquid::kWater);
        setGaz(Gaz::kPoison);

        drawCell();
    }

   protected:
    // Draws cells depending on its member variables.
    void drawCell() {
        // background
        if (active_) {
            *pixmap_ = active_back_->scaled(size_, size_);
        } else {
            *pixmap_ = deactive_back_->scaled(size_, size_);
        }

        QPainter pntr{pixmap_};

        // liquid
        switch (liquid_) {
            case Liquid::kWater:
                pntr.drawPixmap(0, 0, water_->scaled(size_, size_));
                break;
            case Liquid::kOil:
                pntr.drawPixmap(0, 0, oil_->scaled(size_, size_));
                break;
            case Liquid::kNone:
            case Liquid::kEnumLegth:
                break;
        }
        // gaz
        switch (gaz_) {
            case Gaz::kSmoke:
                pntr.drawPixmap(0, 0, smoke_->scaled(size_, size_));
                break;
            case Gaz::kPoison:
                pntr.drawPixmap(0, 0, poison_->scaled(size_, size_));
                break;
            case Gaz::kNone:
            case Gaz::kEnumLength:
                break;
        }

        setPixmap(*pixmap_);
    }
    void mousePressEvent(QMouseEvent* /*e*/) override { emit clicked(); }

    // TODO(clovis): delete this member variable; create default cell background
    bool active_{};
    int size_{};
    QPixmap* pixmap_;
    Liquid liquid_{Liquid::kNone};
    Gaz gaz_{Gaz::kNone};

    // TODO(clovis): place these variables into friend class/namespace?
    // TODO(clovis): rename variables
    inline static const QPixmap* active_back_;
    inline static const QPixmap* deactive_back_;
    inline static const QPixmap* water_;
    inline static const QPixmap* oil_;
    inline static const QPixmap* smoke_;
    inline static const QPixmap* poison_;
};

class ContentWidget : public QScrollArea {
    Q_OBJECT

   public:
    explicit ContentWidget(QWidget* parent,
                           unsigned int size = kDefaultContentWidgetSize)
        : QScrollArea(parent), cells_{size * size} {
        // Scroll widget alignment
        setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        // Content widget
        auto* content = new QWidget{this};

        auto* layout = new QGridLayout{content};
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        content->setLayout(layout);

        for (int i{}; i < size; ++i) {
            for (int j{}; j < size; ++j) {
                auto* cell = new Cell{content};
                layout->addWidget(cell, i, j);

                cells_.push_back(cell);
            }
        }

        setWidget(content);
    }

   protected:
    QList<Cell*> cells_;
};