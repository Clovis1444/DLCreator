#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellLayer.h"

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
            tool_type_ = kClear;
        } else {
            tool_type_ = kNone;
        }
        cell_layer_ = nullptr;

        emit sender_->toolChanged();
    }
    static void setTool(const CellLayer* layer) {
        switch (layer->type()) {
            case CellLayer::kBackground:
                tool_type_ = kBackground;
                break;
            case CellLayer::kLiquid:
                tool_type_ = kLiquid;
                break;
            case CellLayer::kGaz:
                tool_type_ = kGaz;
                break;
            case CellLayer::kEnumLength:
                return;
        }
        cell_layer_ = layer;

        emit sender_->toolChanged();
    }

    static ToolType toolType() { return tool_type_; }
    static const CellLayer* cell_layer() { return cell_layer_; }
    static QString toolName() {
        switch (tool_type_) {
            case kLiquid:
            case kGaz:
            case kBackground:
                return cell_layer_->name();
            case kNone:
                return "No tool";
            case kClear:
                return "Clear";
            case kEnumLength:
                return "?";
        }
    }

    static Tool* signalSender() {
        if (sender_ == nullptr) {
            sender_ = new Tool{};
        }
        return sender_;
    }

   signals:
    void toolChanged();

   protected:
    Tool() = default;

    inline static Tool* sender_{nullptr};

    inline static ToolType tool_type_{};
    inline static const CellLayer* cell_layer_{nullptr};
};
