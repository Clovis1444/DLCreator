#pragma once

#include <qlist.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "../contentWidget/contentWidget.h"

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

   protected:
    QList<ContentWidget*> contents_;
};