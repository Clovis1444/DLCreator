#pragma once

#include <qcontainerfwd.h>
#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellAction.h"

namespace History {
class History : public QObject {
    Q_OBJECT

   public:
    inline static void push(QWidget* w, const CellAction& action) {
        if (w == nullptr) {
            return;
        }

        actions_[w].push_back(action);

        last_action_ = {w, action};

        emit signalSender() -> pushed();
    }

    inline static void remove(QWidget* w) { actions_.remove(w); }

    inline static void undo(QWidget* w, const CellAction& action) {
        if (!actions_.contains(w)) {
            return;
        }

        for (auto i : actions_[w]) {
            if (i == action) {
                i.undo();
                actions_[w].removeOne(i);
                return;
            }
        }
    }

    inline static const QPair<QWidget*, CellAction>& last() {
        return last_action_;
    }

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

    inline static QHash<QWidget*, QList<CellAction>> actions_{};

    inline static QPair<QWidget*, CellAction> last_action_{};
};
}  // namespace History