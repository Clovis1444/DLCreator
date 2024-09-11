#pragma once

#include <qlist.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "contentWidget/contentWidget.h"

class StackedContentWidget : public QStackedWidget {
    Q_OBJECT

   public:
    explicit StackedContentWidget(QWidget* parent) : QStackedWidget{parent} {}
    ~StackedContentWidget() {
        for (auto* i : contents_) {
            delete i;
        }
    }

    // void addWidget() = delete;
    // void InsertWidget() = delete;

    ContentWidget* createContent() {
        auto* content{new ContentWidget{this}};

        contents_.push_back(content);

        addWidget(content);

        return content;
    }
    void deleteContent(ContentWidget* c) {
        for (auto* i : contents_) {
            if (i == c) {
                contents_.removeOne(i);
                delete i;
                return;
            }
        }
    }

    QPair<int, int> gridSize() {
        auto* active_widget{currentWidget()};

        if (active_widget == nullptr) {
            return {0, 0};
        }

        return static_cast<ContentWidget*>(active_widget)->gridSize();
    }

   protected:
    QList<ContentWidget*> contents_;
};