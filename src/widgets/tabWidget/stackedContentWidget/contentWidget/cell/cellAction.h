#pragma once

#include <qcontainerfwd.h>
#include <qforeach.h>
#include <qhash.h>
#include <qlist.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "cell.h"

class CellAction : public QObject {
    Q_OBJECT

   public:
    explicit CellAction(Cell* cell) {
        actions_.append(QPair<Cell*, Cell::CellInfo>{cell, cell->info()});
    }
    explicit CellAction(const QList<Cell*>& cells) {
        for (auto* i : cells) {
            actions_.append(QPair<Cell*, Cell::CellInfo>{i, i->info()});
        }
    }
    CellAction(const CellAction& other)
        : actions_{other.actions_}, action_name_{other.action_name_} {};
    explicit CellAction(const QList<CellAction>& list) {
        for (const auto& i : list) {
            actions_.append(i.actions_);
        }
        registerAction();
    }
    CellAction() = default;

    // TODO(clovis): implement proper action name/action tooltip
    void registerAction() {
        action_name_.clear();
        // action_name_ = "Change of ";
        action_name_ += QString::number(actions_.count());
        // action_name_ += " cells";
    }

    void undo() {
        for (auto i : actions_) {
            i.first->setLayer(i.second);
        }
    }

    QString text() const { return action_name_; }

    bool changed() {
        foreach (const auto& i, actions_) {
            if (i.first->background() != i.second.background ||
                i.first->liquid() != i.second.liquid ||
                i.first->gaz() != i.second.gaz) {
                return true;
            }
        }
        return false;
    }

    CellAction& operator=(const CellAction& other) {
        if (this == &other) {
            return *this;
        }

        actions_ = other.actions_;
        action_name_ = other.action_name_;

        return *this;
    }

    bool operator==(const CellAction& other) const {
        return actions_ == other.actions_ && action_name_ == other.action_name_;
    }

   protected:
    QList<QPair<Cell*, Cell::CellInfo>> actions_;

    QString action_name_;
};