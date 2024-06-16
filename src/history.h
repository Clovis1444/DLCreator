#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellAction.h"

namespace History {
class History : public QObject {
    Q_OBJECT

   public:
    inline static void push(const CellAction& action) {
        actions_.push_back(action);

        emit signalSender() -> pushed();
    }

    inline static void undo(const CellAction& action) {
        for (auto i : actions_) {
            if (i == action) {
                i.undo();
                actions_.removeOne(i);
                return;
            }
        }
    }

    inline static CellAction last() { return actions_.last(); }

    inline static History* signalSender() {
        if (sender_ == nullptr) {
            sender_ = new History{};
        }
        return sender_;
    }

   signals:
    void pushed();

   protected:
    inline static History* sender_{nullptr};

    inline static QList<CellAction> actions_{};
};
}  // namespace History