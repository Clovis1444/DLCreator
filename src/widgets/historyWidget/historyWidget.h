#pragma once

#include <qboxlayout.h>
#include <qframe.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>

#include "../../history.h"
#include "cellActionWidget.h"

// TODO(clovis): make it ScrollWidget
class HistoryWidget : public QFrame {
    Q_OBJECT

   public:
    explicit HistoryWidget(QWidget* parent)
        : QFrame{parent}, layout_{new QVBoxLayout{this}} {
        setFixedWidth(k_widget_width_);

        setLayout(layout_);
        layout_->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

        QObject::connect(History::History::signalSender(),
                         &History::History::pushed, this,
                         &HistoryWidget::onHistoryPushed);
    }

    void addWidget(const CellAction& action) {
        auto* widget{new CellActionWidget{this, action}};
        layout_->insertWidget(0, widget);
        widgets_.push_back(widget);

        QObject::connect(
            widget, &CellActionWidget::mousePressed, this,
            [this, widget] { undoActionAndDeleteActionWidget(widget); });
    }

   protected slots:
    void onHistoryPushed() { addWidget(History::History::last()); };

    void undoActionAndDeleteActionWidget(CellActionWidget* w) {
        History::History::undo(w->action());

        removeWidget(w);
    };

    void removeWidget(CellActionWidget* w) {
        widgets_.removeOne(w);
        delete w;
    }

   protected:
    QVBoxLayout* layout_;
    QList<CellActionWidget*> widgets_;

    inline static int k_widget_width_{100};
};