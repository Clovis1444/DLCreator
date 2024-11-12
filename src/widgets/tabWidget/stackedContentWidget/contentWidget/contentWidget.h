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

#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/gridManager.h"

class ContentWidget : public QWidget {
    Q_OBJECT

   public:
    explicit ContentWidget(QWidget* parent,
                           int size = kDefaultContentWidgetSize)
        : QWidget{parent},
          layout_{new QGridLayout{this}},
          content_{new GridManager{this, size}} {
        setLayout(layout_);
        layout_->addWidget(content_);
    }
    ContentWidget(QWidget* parent, GridManager* gm)
        : QWidget(parent), layout_{new QGridLayout{this}}, content_{gm} {
        setLayout(layout_);
        layout_->addWidget(content_);
    }

    QPair<int, int> gridSize() const { return content_->gridSize(); }

    const GridManager* gridManager() { return content_; };
    QWidget* widget() { return content_; }

   protected:
    // void keyPressEvent(QKeyEvent* e) override {
    //     switch (e->key()) {
    //         case Qt::Key_Escape:
    //             if (content_->hasSelection()) {
    //                 content_->unselectCells();
    //             } else {
    //                 Tool::setTool();
    //             }
    //             break;
    //         case Qt::Key_Delete:
    //             content_->clearSelectedCells();
    //             break;
    //         case Qt::Key_Shift:
    //             content_->SwitchExpandButtons(false);
    //             break;
    //         default:
    //             break;
    //     }
    //     if (e->key() == Qt::Key_Escape) {
    //         content_->unselectCells();
    //     };
    //
    //     QScrollArea::keyPressEvent(e);
    // }
    //
    // void keyReleaseEvent(QKeyEvent* e) override {
    //     if (e->key() == Qt::Key_Shift) {
    //         content_->SwitchExpandButtons(true);
    //     }
    //
    //     QScrollArea::keyReleaseEvent(e);
    // }

   private:
    QGridLayout* layout_;

    GridManager* content_;

    inline static constexpr int kDefaultContentWidgetSize{10};
};
