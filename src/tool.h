#pragma once

#include <qdebug.h>

#include "widgets/cell/cellLayer.h"

class Tool {
   public:
    Tool() = delete;
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
            tool_type_ = kClear;
        } else {
            tool_type_ = kNone;
        }
        cell_layer_ = nullptr;
    }
    static void setTool(const Liquid* liquid) {
        tool_type_ = kLiquid;
        cell_layer_ = liquid;
    }
    static void setTool(const Gaz* gaz) {
        tool_type_ = kGaz;
        cell_layer_ = gaz;
    }
    static void setTool(const Background* background) {
        tool_type_ = kBackground;
        cell_layer_ = background;
    }

    static ToolType toolType() { return tool_type_; }
    static const CellLayer* cell_layer() { return cell_layer_; }

   protected:
    inline static ToolType tool_type_{};
    inline static const CellLayer* cell_layer_{nullptr};
};
