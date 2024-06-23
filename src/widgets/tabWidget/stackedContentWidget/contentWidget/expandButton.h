#pragma once

#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>

class ExpandButton : public QPushButton {
    Q_OBJECT

   public:
    // TODO(clovis): set proper expanding
    explicit ExpandButton(QWidget* parent, bool expand_horizontal)
        : QPushButton{parent} {
        if (expand_horizontal) {
            setFixedHeight(kSize);
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        } else {
            setFixedWidth(kSize);
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        }

        setPositive(true);
    }

    void setPositive(bool positive) {
        if (positive) {
            setText("+");
            setToolTip("Add more cells");
        } else {
            setText("-");
            setToolTip("Remove cells");
        }
    }

   protected:
    inline static constexpr int kSize{50};
};