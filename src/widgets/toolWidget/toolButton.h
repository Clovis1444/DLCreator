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
    ToolButton(const CellLayer* cell_layer, const QString& name,
               QWidget* parent)
        : QPushButton{name, parent}, cell_layer_{cell_layer} {
        switch (cell_layer->type()) {
            case CellLayer::kBackground:
                tool_type_ = Tool::kBackground;
                break;
            case CellLayer::kLiquid:
                tool_type_ = Tool::kLiquid;
                break;
            case CellLayer::kGaz:
                tool_type_ = Tool::kGaz;
                break;
            case CellLayer::kEnumLength:
                break;
        }

        QObject::connect(this, &QPushButton::clicked, this,
                         &ToolButton::onClicked);
    }

   private slots:
    void onClicked() {
        switch (tool_type_) {
            case Tool::kNone:
                Tool::setTool();
                break;
            case Tool::kBackground:
            case Tool::kLiquid:
            case Tool::kGaz:
                Tool::setTool(cell_layer_);
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