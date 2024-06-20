#pragma once

#include <qstackedwidget.h>
#include <qtmetamacros.h>

#include "historyWidget/historyWidget.h"

class StackedHistoryWidget : public QStackedWidget {
    Q_OBJECT

   public:
    explicit StackedHistoryWidget(QWidget* parent) : QStackedWidget{parent} {}
    ~StackedHistoryWidget() {
        for (auto* i : histories_) {
            delete i;
        }
    }

    HistoryWidget* createHistory(QWidget* target) {
        auto* history{new HistoryWidget{this, target}};

        histories_.push_back(history);

        addWidget(history);

        return history;
    }
    void deleteHistory(HistoryWidget* h) {
        histories_.removeOne(h);
        delete h;
    }

   protected:
    QList<HistoryWidget*> histories_;
};