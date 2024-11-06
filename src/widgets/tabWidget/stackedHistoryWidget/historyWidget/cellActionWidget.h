#pragma once

#include <qboxlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qtmetamacros.h>

#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellAction.h"

class CellActionWidget : public QLabel {
    Q_OBJECT

   public:
    explicit CellActionWidget(QWidget* parent, const CellAction& action)
        : QLabel{parent}, action_{action}, layout_{new QHBoxLayout{this}} {
        setLayout(layout_);
        layout_->setAlignment(Qt::AlignCenter);

        setFixedSize(k_widget_size_, k_widget_size_);
        setFrameShape(Box);
        setToolTip(action_.text());
        setText(action_.text());
    }

    const CellAction& action() { return action_; }

   signals:
    void mousePressed();

   protected:
    void mousePressEvent(QMouseEvent* e) override {
        if (e->button() == Qt::LeftButton) {
            emit mousePressed();
            return;
        }

        QLabel::mousePressEvent(e);
    }

    const CellAction action_;
    QHBoxLayout* layout_;

    inline static int k_widget_size_{50};
};
