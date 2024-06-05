#pragma once

#include <qpushbutton.h>
#include <qtmetamacros.h>

#include "../../tool.h"
#include "../tabWidget/stackedContentWidget/contentWidget/cell/cellLayer.h"

class ToolButton : public QPushButton {
    Q_OBJECT

   public:
    ToolButton(const QString& name, QWidget* parent, bool clear_tool = false)
        : QPushButton{name, parent} {
        if (clear_tool) {
            tool_type_ = Tool::kClear;
        } else {
            tool_type_ = Tool::kNone;
        }
        QObject::connect(this, &QPushButton::clicked, this,
                         &ToolButton::onClicked);
    }
    ToolButton(const Liquid* cell_layer, const QString& name, QWidget* parent)
        : QPushButton{name, parent}, cell_layer_{cell_layer} {
        tool_type_ = Tool::kLiquid;
        QObject::connect(this, &QPushButton::clicked, this,
                         &ToolButton::onClicked);
    }
    ToolButton(const Gaz* cell_layer, const QString& name, QWidget* parent)
        : QPushButton{name, parent}, cell_layer_{cell_layer} {
        tool_type_ = Tool::kGaz;
        QObject::connect(this, &QPushButton::clicked, this,
                         &ToolButton::onClicked);
    }
    ToolButton(const Background* cell_layer, const QString& name,
               QWidget* parent)
        : QPushButton{name, parent}, cell_layer_{cell_layer} {
        tool_type_ = Tool::kBackground;
        QObject::connect(this, &QPushButton::clicked, this,
                         &ToolButton::onClicked);
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
            case Tool::kBackground:
                Tool::setTool(static_cast<const Background*>(cell_layer_));
                break;
            case Tool::kEnumLength:
                break;
        }
    }

   protected:
    Tool::ToolType tool_type_{};
    const CellLayer* cell_layer_{nullptr};
};