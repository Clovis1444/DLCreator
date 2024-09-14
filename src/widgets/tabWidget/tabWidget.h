#pragma once

#include <qboxlayout.h>
#include <qgridlayout.h>
#include <qlist.h>
#include <qobject.h>
#include <qscrollarea.h>
#include <qsizepolicy.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "src/widgets/tabWidget/stackedHistoryWidget/stackedHistoryWidget.h"
#include "stackedContentWidget/contentWidget/contentWidget.h"
#include "stackedContentWidget/stackedContentWidget.h"
#include "tabScrollArea/tabScrollArea.h"

class TabWidget : public QWidget {
    Q_OBJECT

   public:
    explicit TabWidget(QWidget* parent)
        : QWidget{parent},
          stacked_content_widget_{new StackedContentWidget{this}},
          stacked_history_widget_{new StackedHistoryWidget{this}},
          tabs_widget_{new TabScrollArea{this}},
          layout_{new QGridLayout{this}} {
        // Layout
        setLayout(layout_);
        layout_->setSpacing(0);
        layout_->setContentsMargins(0, 0, 0, 0);

        // Tabs
        layout_->addWidget(tabs_widget_, 0, 0, 1, 1);
        tabs_widget_->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Minimum);

        // Content
        layout_->addWidget(stacked_content_widget_, 1, 0, 1, 1);
        stacked_content_widget_->setSizePolicy(QSizePolicy::Expanding,
                                               QSizePolicy::Expanding);

        //    History
        layout_->addWidget(stacked_history_widget_, 0, 1, 2, 1);
        stacked_history_widget_->setSizePolicy(QSizePolicy::Fixed,
                                               QSizePolicy::Expanding);
    }
    ~TabWidget() {
        delete stacked_content_widget_;
        delete stacked_history_widget_;
        delete tabs_widget_;
    }

    void createTab(const QString& name = "New tab") {
        auto* tab_button{tabs_widget_->createButton(name)};
        auto* content{stacked_content_widget_->createContent()};
        auto* history{
            stacked_history_widget_->createHistory(content->widget())};

        TabWidgetItem item{tab_button, content, history};
        items_.push_back(item);

        // Set new tab as active tab
        setCurrentItem(item);

        QObject::connect(tab_button, &TabButton::clicked, this,
                         &TabWidget::onTabClicked);
        QObject::connect(tab_button, &TabButton::closePressed, this,
                         &TabWidget::onTabClose);
    }
    void createTab(CellCollection* cc, const QString& name = "New tab") {
        if (cc == nullptr) return;

        auto* tab_button{tabs_widget_->createButton(name)};
        auto* content{stacked_content_widget_->createContent(cc)};
        auto* history{
            stacked_history_widget_->createHistory(content->widget())};

        TabWidgetItem item{tab_button, content, history};
        items_.push_back(item);

        // Set new tab as active tab
        setCurrentItem(item);

        QObject::connect(tab_button, &TabButton::clicked, this,
                         &TabWidget::onTabClicked);
        QObject::connect(tab_button, &TabButton::closePressed, this,
                         &TabWidget::onTabClose);
    }

    void setTabsVisible(bool visible) { tabs_widget_->setVisible(visible); }
    void setHistoryVisible(bool visible) {
        stacked_history_widget_->setVisible(visible);
    }
    void setContentVisible(bool visible) {
        stacked_content_widget_->setVisible(visible);
    }

    QPair<int, int> gridSize() { return stacked_content_widget_->gridSize(); }

    const CellCollection* cellCollection() {
        return stacked_content_widget_->cellCollection();
    }

   protected slots:
    void onTabClicked() {
        for (auto i : items_) {
            if (i.button == sender()) {
                setCurrentItem(i);
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
        HistoryWidget* history;

        friend bool operator==(const TabWidgetItem& first,
                               const TabWidgetItem& second) = default;
    };

    void setCurrentItem(const TabWidgetItem& i) {
        stacked_content_widget_->setCurrentWidget(i.content);
        stacked_history_widget_->setCurrentWidget(i.history);
    }

    void deleteItem(const TabWidgetItem& i) {
        stacked_content_widget_->deleteContent(i.content);
        stacked_history_widget_->deleteHistory(i.history);
        tabs_widget_->deleteButton(i.button);

        items_.removeOne(i);
    }

    StackedContentWidget* stacked_content_widget_;
    StackedHistoryWidget* stacked_history_widget_;
    TabScrollArea* tabs_widget_;
    QGridLayout* layout_;
    QList<TabWidgetItem> items_;
};