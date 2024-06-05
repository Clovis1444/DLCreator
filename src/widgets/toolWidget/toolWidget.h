#pragma once

#include <qboxlayout.h>
#include <qframe.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "collapsibleSection.h"
#include "toolButton.h"

class ToolWidget : public QFrame {
    Q_OBJECT

   public:
    explicit ToolWidget(QWidget* parent)
        : QFrame{parent},
          layout_{new QVBoxLayout{this}},
          layout_refresh_{new QHBoxLayout{}},
          refresh_button_{new QPushButton{"Refresh", this}},
          layout_tools_{new QVBoxLayout{}} {
        // setFixedWidth(300);

        // Main layout
        layout_->setContentsMargins(0, 0, 0, 0);
        layout_->setSpacing(10);
        setLayout(layout_);

        // Refresh button layout
        layout_->addLayout(layout_refresh_);

        layout_refresh_->setContentsMargins(0, 10, 10, 0);
        layout_refresh_->setSpacing(0);
        layout_refresh_->setAlignment(Qt::AlignRight);

        refresh_button_->setFixedSize(50, 50);
        layout_refresh_->addWidget(refresh_button_);

        // Tools layout
        layout_->addLayout(layout_tools_);
        layout_tools_->setContentsMargins(10, 0, 10, 10);
        layout_tools_->setAlignment(Qt::AlignTop);
        loadTools();

        // Connects
        QObject::connect(refresh_button_, &QPushButton::clicked, this,
                         &ToolWidget::onRefreshClicked);
    }

   protected slots:
    void onRefreshClicked() { loadTools(); }

   protected:
    void loadTools() {
        deleteAllTools();
        loadCellLayersFromJson();

        // Basic tools
        auto* basic{new CollapsibleSection{"Basic tools", this}};
        basic->addContent(new ToolButton{"Selection", basic});
        basic->addContent(new ToolButton{"Clear tool", basic, true});
        layout_tools_->insertWidget(layout_tools_->count(), basic);
        tool_sections_.push_back(basic);

        // Backgrounds
        auto* backgrounds{new CollapsibleSection{"Backgrounds", this}};
        for (auto* i : Background::list()) {
            backgrounds->addContent(new ToolButton{i, i->name(), backgrounds});
        }
        layout_tools_->insertWidget(layout_tools_->count(), backgrounds);
        tool_sections_.push_back(backgrounds);

        // Liquids
        auto* liquids{new CollapsibleSection{"Liquids", this}};
        for (auto* i : Liquid::list()) {
            liquids->addContent(new ToolButton{i, i->name(), liquids});
        }
        layout_tools_->insertWidget(layout_tools_->count(), liquids);
        tool_sections_.push_back(liquids);

        // Gasses
        auto* gasses{new CollapsibleSection{"Gasses", this}};
        for (auto* i : Gaz::list()) {
            gasses->addContent(new ToolButton{i, i->name(), gasses});
        }
        layout_tools_->insertWidget(layout_tools_->count(), gasses);
        tool_sections_.push_back(gasses);
    }

    void deleteAllTools() {
        for (auto* i : tool_sections_) {
            delete i;
        }

        tool_sections_.clear();
    };

    QVBoxLayout* layout_;

    QHBoxLayout* layout_refresh_;
    QPushButton* refresh_button_;

    QVBoxLayout* layout_tools_;
    QList<CollapsibleSection*> tool_sections_;
};