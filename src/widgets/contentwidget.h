#pragma once

#include <qabstractscrollarea.h>
#include <qboxlayout.h>
#include <qgridlayout.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>
#include <qwidget.h>

inline constexpr int kCellSize{50};
inline constexpr int kDefaultContentWidgetSize{10};

class Cell : public QPushButton {
    Q_OBJECT

   public:
    explicit Cell(QWidget* parent, int cell_size = kCellSize)
        : QPushButton{parent} {
        setFixedWidth(cell_size);
        setFixedHeight(cell_size);

        QObject::connect(this, &QPushButton::clicked, this, &Cell::onClicked);
    }

   public slots:
    void onClicked() {
        if (active_) {
            setText("");
        } else {
            setText("X");
        }

        active_ = !active_;
    }

   protected:
    bool active_{};
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