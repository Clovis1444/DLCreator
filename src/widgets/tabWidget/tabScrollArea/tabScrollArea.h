#pragma once

#include <qboxlayout.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qtmetamacros.h>

#include "tabButton.h"

// TODO(clovis): fix TabScroll area size
class TabScrollArea : public QScrollArea {
    Q_OBJECT

   public:
    explicit TabScrollArea(QWidget* parent)
        : QScrollArea{parent}, widget_{new QWidget{this}} {
        setWidget(widget_);
        setWidgetResizable(true);

        auto* layout{new QHBoxLayout{widget_}};
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setAlignment(Qt::AlignLeft);
    }
    ~TabScrollArea() {
        for (auto* i : buttons_) {
            delete i;
        }
    }

    // void addWidget() = delete;
    // void InsertWidget() = delete;

    TabButton* createButton(const QString& name = "New button") {
        auto* tab_button{new TabButton{widget_, name}};

        buttons_.push_back(tab_button);

        widget_->layout()->addWidget(tab_button);

        return tab_button;
    }
    void deleteButton(TabButton* b) {
        for (auto* i : buttons_) {
            if (i == b) {
                buttons_.removeOne(i);
                delete i;
                return;
            }
        }
    }

   protected:
    QList<TabButton*> buttons_;
    QHBoxLayout* layout_;
    QWidget* widget_;
};