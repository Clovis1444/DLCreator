#pragma once

#include <qboxlayout.h>
#include <qlist.h>
#include <qscrollarea.h>
#include <qtmetamacros.h>

#include "../tabButton.h"

class TabScrollArea : public QScrollArea {
    Q_OBJECT

   public:
    explicit TabScrollArea(QWidget* parent)
        : QScrollArea{parent}, layout_{new QHBoxLayout{this}} {
        setLayout(layout_);

        layout_->setAlignment(Qt::AlignLeft);
    }
    ~TabScrollArea() {
        for (auto* i : buttons_) {
            delete i;
        }
    }

    // void addWidget() = delete;
    // void InsertWidget() = delete;

    TabButton* createButton(const QString& name = "New button") {
        auto* tab_button{new TabButton{this, name}};

        buttons_.push_back(tab_button);

        layout_->addWidget(tab_button);

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
};