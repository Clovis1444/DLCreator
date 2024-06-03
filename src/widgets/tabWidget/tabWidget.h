#pragma once

#include <qboxlayout.h>
#include <qlist.h>
#include <qobject.h>
#include <qscrollarea.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

// #include "../../document.h"
#include "../contentWidget/contentWidget.h"
#include "stackedContentWidget.h"
#include "tabScrollArea.h"

class TabWidget : public QWidget {
    Q_OBJECT

   public:
    explicit TabWidget(QWidget* parent)
        : QWidget{parent},
          content_widget_{new StackedContentWidget{this}},
          tabs_widget_{new TabScrollArea{this}},
          layout_{new QVBoxLayout{this}} {
        // Layout
        setLayout(layout_);
        layout_->setSpacing(0);
        layout_->setContentsMargins(0, 0, 0, 0);

        // Tabs
        layout_->addWidget(tabs_widget_);

        // Content
        layout_->addWidget(content_widget_);
    }
    ~TabWidget() {
        delete content_widget_;
        delete tabs_widget_;
    }

    void createTab(const QString& name = "New tab") {
        auto* tab_button{tabs_widget_->createButton(name)};
        auto* content{content_widget_->createContent()};

        TabWidgetItem item{tab_button, content};
        items_.push_back(item);

        QObject::connect(tab_button, &TabButton::clicked, this,
                         &TabWidget::onTabClicked);
        QObject::connect(tab_button, &TabButton::closePressed, this,
                         &TabWidget::onTabClose);
    }
    void deleteTab() {}

   protected slots:
    void onTabClicked() {
        for (auto i : items_) {
            if (i.button == sender()) {
                content_widget_->setCurrentWidget(i.content);
                return;
            }
        }
    }
    void onTabClose() {
        for (auto i : items_) {
            if (i.button == sender()) {
                deleteItem(i);
                return;
            }
        }
    }

   protected:
    struct TabWidgetItem {
        TabButton* button;
        ContentWidget* content;

        friend bool operator==(const TabWidgetItem& first,
                               const TabWidgetItem& second) = default;
    };

    void deleteItem(TabWidgetItem i) {
        content_widget_->deleteContent(i.content);
        tabs_widget_->deleteButton(i.button);

        items_.removeOne(i);
    }

    StackedContentWidget* content_widget_;
    TabScrollArea* tabs_widget_;
    QVBoxLayout* layout_;
    QList<TabWidgetItem> items_;
    // QList<Document*> documents_;
};