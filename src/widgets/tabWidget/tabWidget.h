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

    void createTab(const QString& name = kDefaultNewTabName) {
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
    void createTab(CellCollection* cc, const QString& name = kDefaultNewTabName,
                   const QString& file_path = "") {
        if (cc == nullptr) return;

        auto* tab_button{tabs_widget_->createButton(name)};
        auto* content{stacked_content_widget_->createContent(cc)};
        auto* history{
            stacked_history_widget_->createHistory(content->widget())};

        TabWidgetItem item{tab_button, content, history, file_path};
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

    void setTabName(const QString& name) {
        // Tab name must contains at least 1 symbol
        if (name.isEmpty()) return;

        TabWidgetItem item{currentItem()};

        if (item == TabWidgetItem{}) return;

        // Set new name
        item.button->setText(name);
    }

    void setTabFilePath(const QString& file_path) {
        TabWidgetItem item{currentItem()};

        if (item == TabWidgetItem{}) return;

        for (auto& i : items_) {
            if (i == item) {
                i.file_path = file_path;
                break;
            }
        }
    }

    QPair<int, int> gridSize() { return stacked_content_widget_->gridSize(); }

    const CellCollection* cellCollection() {
        return stacked_content_widget_->cellCollection();
    }

    int tabs_count() const { return static_cast<int>(items_.count()); }

    QString tabName() const {
        TabWidgetItem item{currentItem()};

        if (item.button == nullptr) {
            return "";
        }

        return item.button->text();
    }

    QString tabFilePath() const {
        TabWidgetItem item{currentItem()};

        return item.file_path;
    }

    // If file already loaded makes its tab active and returns TRUE.
    // If there is no such file returns FALSE
    bool switchToFileTabIfLoaded(const QString& file_path) {
        // NOLINTNEXTLINE
        for (const auto& i : items_) {
            if (i.file_path == file_path) {
                setCurrentItem(i);
                return true;
            }
        }

        return false;
    }

   protected slots:
    void onTabClicked() {
        for (const auto& i : items_) {
            if (i.button == sender()) {
                setCurrentItem(i);
                return;
            }
        }
    }
    void onTabClose() {
        for (const auto& i : items_) {
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
        QString file_path{""};

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

    TabWidgetItem currentItem() const {
        for (auto i : items_) {
            if (i.content == stacked_content_widget_->currentWidget()) {
                return i;
            }
        }

        return TabWidgetItem{};
    }

    StackedContentWidget* stacked_content_widget_;
    StackedHistoryWidget* stacked_history_widget_;
    TabScrollArea* tabs_widget_;
    QGridLayout* layout_;
    QList<TabWidgetItem> items_;

    inline static const QString kDefaultNewTabName{"Untitled map"};
};