#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "widgets/contentWidget/cell/cellLayer.h"

class Tool : public QObject {
    Q_OBJECT

   public:
    Tool(const Tool&) = delete;

    enum ToolType {
        kNone,
        kLiquid,
        kGaz,
        kClear,
        kBackground,
        // Add new entries here
        kEnumLength,
    };

    static void setTool(bool set_clear = false) {
        if (set_clear) {
            tool_->tool_type_ = kClear;
        } else {
            tool_->tool_type_ = kNone;
        }
        tool_->cell_layer_ = nullptr;

        emit tool_->toolChanged();
    }
    static void setTool(const Liquid* liquid) {
        tool_->tool_type_ = kLiquid;
        tool_->cell_layer_ = liquid;

        emit tool_->toolChanged();
    }
    static void setTool(const Gaz* gaz) {
        tool_->tool_type_ = kGaz;
        tool_->cell_layer_ = gaz;

        emit tool_->toolChanged();
    }
    static void setTool(const Background* background) {
        tool_->tool_type_ = kBackground;
        tool_->cell_layer_ = background;

        emit tool_->toolChanged();
    }

    static ToolType toolType() { return tool_->tool_type_; }
    static const CellLayer* cell_layer() { return tool_->cell_layer_; }
    static QString toolName() {
        switch (tool_->tool_type_) {
            case kLiquid:
            case kGaz:
            case kBackground:
                return tool_->cell_layer_->name();
            case kNone:
                return "No tool";
            case kClear:
                return "Clear";
            case kEnumLength:
                return "?";
        }
    }

    static Tool* obj() { return tool_; }

    static void init() {
        if (tool_ == nullptr) {
            tool_ = new Tool{};
        }
    }

   signals:
    void toolChanged();

   protected:
    Tool() = default;

    // TODO(clovis): not cool. fix it?
    inline static Tool* tool_{nullptr};

    ToolType tool_type_{};
    const CellLayer* cell_layer_{nullptr};
};
