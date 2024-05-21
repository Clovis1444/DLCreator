#pragma once

#include <qboxlayout.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>
#include <qtoolbutton.h>
#include <qwindowdefs.h>

class CollapsibleSection : public QFrame {
    Q_OBJECT

   public:
    CollapsibleSection(const QString& text, QWidget* parent) : QFrame{parent} {
        setFrameShape(Shape::Box);

        auto* layout{new QVBoxLayout{this}};
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        setLayout(layout);

        button_ = new QToolButton{this};
        button_->setToolButtonStyle(
            Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
        button_->setText(text);
        button_->setArrowType(Qt::ArrowType::DownArrow);
        button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        content_ = new QFrame{this};
        content_->setFrameShape(Shape::Box);
        auto* content_layout = new QHBoxLayout{content_};
        content_->setLayout(content_layout);

        layout->addWidget(button_);
        layout->addWidget(content_);

        QObject::connect(button_, &QToolButton::clicked, this,
                         &CollapsibleSection::onButtonClicked);
    }
    ~CollapsibleSection() {
        delete button_;
        delete content_;
    }

    void addContent(QWidget* widget) { content_->layout()->addWidget(widget); }

   private slots:
    void onButtonClicked() {
        collapsed_ = !collapsed_;
        if (collapsed_) {
            button_->setArrowType(Qt::ArrowType::RightArrow);
            content_->hide();
        } else {
            button_->setArrowType(Qt::ArrowType::DownArrow);
            content_->show();
        }
    };

   protected:
    bool collapsed_{};
    QToolButton* button_;
    QFrame* content_;
};