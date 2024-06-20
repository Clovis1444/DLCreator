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

#include "cellCollection.h"

class ContentWidget : public QScrollArea {
    Q_OBJECT

   public:
    explicit ContentWidget(QWidget* parent,
                           unsigned int size = kDefaultContentWidgetSize)
        : QScrollArea(parent), content_{new CellCollection{this, size}} {
        // CellColection widget alignment
        setAlignment(Qt::AlignHCenter | Qt::AlignCenter);

        setWidget(content_);
    }

   protected:
    void keyPressEvent(QKeyEvent* e) override {
        switch (e->key()) {
            case Qt::Key_Escape:
                if (content_->hasSelection()) {
                    content_->unselectCells();
                } else {
                    Tool::setTool();
                }
                break;
            case Qt::Key_Delete:
                content_->clearSelectedCells();
                break;
            default:
                break;
        }
        if (e->key() == Qt::Key_Escape) {
            content_->unselectCells();
        };

        QAbstractScrollArea::keyPressEvent(e);
    }

   private:
    CellCollection* content_;

    inline static constexpr int kDefaultContentWidgetSize{10};
};