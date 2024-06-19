#pragma once

#include <qboxlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class NameFrame : public QFrame {
    Q_OBJECT

   public:
    NameFrame(const QString& name, QWidget* parent)
        : QFrame{parent},
          layout_{new QHBoxLayout{this}},
          name_label_{new QLabel{name, this}} {
        // Size
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        setFixedHeight(kHeight);

        // Style
        setFrameStyle(QFrame::Box);
        setFrameShadow(QFrame::Plain);
        setLineWidth(3);

        // Layout
        setLayout(layout_);
        layout_->setContentsMargins(0, 0, 0, 0);
        layout_->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);

        // Label
        name_label_->setFont(kFont);
        layout_->addWidget(name_label_);
    }

   protected:
    QHBoxLayout* layout_;
    QLabel* name_label_;
    // TODO(clovis): add close button?

    inline static constexpr int kHeight{30};
    inline static const QFont kFont{"Arial", 12};
};