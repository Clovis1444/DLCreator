#pragma once

#include <qboxlayout.h>
#include <qframe.h>
#include <qlayoutitem.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>

#include "cellActionWidget.h"
#include "src/history.h"
#include "src/widgets/components/nameFrame.h"

class HistoryWidget : public QScrollArea {
    Q_OBJECT

   public:
    HistoryWidget(QWidget* parent, QWidget* history_target)
        : QScrollArea{parent},
          name_frame_{new NameFrame{kName, this}},
          layout_{new QVBoxLayout{this}},
          content_{new QFrame{this}},
          content_layout_{new QVBoxLayout{content_}},
          history_target_{history_target} {
        // Layout
        setLayout(layout_);
        layout_->setContentsMargins(0, 0, 0, 0);
        layout_->setAlignment(Qt::AlignTop);
        layout_->addWidget(name_frame_);

        setFixedWidth(kWidgetWidth);

        // Content layout
        content_layout_->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        int top_margin{name_frame_->height() +
                       content_layout_->contentsMargins().top()};
        content_layout_->setContentsMargins(0, top_margin, 0, 0);
        content_->setLayout(content_layout_);

        // Content
        setWidgetResizable(true);
        setWidget(content_);

        QObject::connect(History::History::sender(history_target_),
                         &History::History::pushed, this,
                         &HistoryWidget::onHistoryPushed);
    }
    ~HistoryWidget() override { History::History::remove(history_target_); }

    void addWidget(const CellAction& action) {
        auto* widget{new CellActionWidget{content_, action}};
        content_layout_->insertWidget(0, widget);
        widgets_.push_back(widget);

        QObject::connect(
            widget, &CellActionWidget::mousePressed, this,
            [this, widget] { undoActionAndDeleteActionWidget(widget); });
    }

   protected slots:
    void onHistoryPushed() {
        addWidget(History::History::last(history_target_));
    };

    void undoActionAndDeleteActionWidget(CellActionWidget* w) {
        History::History::undo(history_target_, w->action());

        removeWidget(w);
    };

    void removeWidget(CellActionWidget* w) {
        widgets_.removeOne(w);
        delete w;
    }

   protected:
    NameFrame* name_frame_;
    QVBoxLayout* layout_;
    QFrame* content_;
    QVBoxLayout* content_layout_;
    QList<CellActionWidget*> widgets_;
    QWidget* history_target_;

    inline static constexpr int kWidgetWidth{150};
    inline static const QString kName{"Action History"};
};
