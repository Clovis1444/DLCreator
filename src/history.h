#pragma once

#include <qassert.h>
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

        emit senders_[w]->pushed();
    }

    inline static void remove(QWidget* w) {
        actions_.remove(w);
        delete senders_[w];
        senders_.remove(w);
    }

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

    inline static const CellAction& last(QWidget* w) {
        Q_ASSERT(actions_.contains(w) && !actions_[w].isEmpty());

        return actions_[w].last();
    }

    inline static History* sender(QWidget* w) {
        if (!senders_.contains(w)) {
            senders_.insert(w, new History{});
        }

        return senders_[w];
    }

   signals:
    void pushed();

   protected:
    inline static History* sender_{nullptr};

    inline static QHash<QWidget*, QList<CellAction>> actions_{};
    inline static QHash<QWidget*, History*> senders_{};
};
}  // namespace History