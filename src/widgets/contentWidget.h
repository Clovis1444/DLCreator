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

#include "cell/cell.h"
#include "cellCollection.h"

inline constexpr int kDefaultContentWidgetSize{10};

class ContentWidget : public QScrollArea {
    Q_OBJECT

   public:
    explicit ContentWidget(QWidget* parent,
                           unsigned int size = kDefaultContentWidgetSize)
        : QScrollArea(parent), cells_{size * size} {
        // Scroll widget alignment
        setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        // Content widget
        auto* content = new CellCollection{this};
        content_ = content;

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

   private:
    QList<Cell*> cells_;
    CellCollection* content_;
};