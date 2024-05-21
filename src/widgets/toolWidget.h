#pragma once

#include <qpushbutton.h>
#include <qtmetamacros.h>

#include "../tool.h"
#include "cell/cellLayer.h"

class ToolWidget : public QPushButton {
    Q_OBJECT

   public:
    ToolWidget(const QString& name, QWidget* parent, bool clear_tool = false)
        : QPushButton{name, parent} {
        if (clear_tool) {
            tool_type_ = Tool::kClear;
        } else {
            tool_type_ = Tool::kNone;
        }
        QObject::connect(this, &QPushButton::clicked, this,
                         &ToolWidget::onClicked);
    }
    ToolWidget(const Liquid* cell_layer, const QString& name, QWidget* parent)
        : QPushButton{name, parent}, cell_layer_{cell_layer} {
        tool_type_ = Tool::kLiquid;
        QObject::connect(this, &QPushButton::clicked, this,
                         &ToolWidget::onClicked);
    }
    ToolWidget(const Gaz* cell_layer, const QString& name, QWidget* parent)
        : QPushButton{name, parent}, cell_layer_{cell_layer} {
        tool_type_ = Tool::kGaz;
        QObject::connect(this, &QPushButton::clicked, this,
                         &ToolWidget::onClicked);
    }

   private slots:
    void onClicked() {
        switch (tool_type_) {
            case Tool::kNone:
                Tool::setTool();
                break;
            case Tool::kLiquid:
                Tool::setTool(static_cast<const Liquid*>(cell_layer_));
                break;
            case Tool::kGaz:
                Tool::setTool(static_cast<const Gaz*>(cell_layer_));
                break;
            case Tool::kClear:
                Tool::setTool(true);
                break;
            case Tool::kEnumLength:
                break;
        }
    }

   protected:
    Tool::ToolType tool_type_{};
    const CellLayer* cell_layer_{nullptr};
};