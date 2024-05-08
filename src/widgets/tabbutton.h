#pragma once

#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class TabButton : public QPushButton {
    Q_OBJECT

   public:
    explicit TabButton(QWidget* parent, const QString& text = "")
        : QPushButton{text, parent} {
        auto* layout{new QHBoxLayout{this}};
        layout->setAlignment(Qt::AlignRight);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 6, 0);
        this->setLayout(layout);

        auto* close_button{new QPushButton{"", this}};
        close_button->setFixedSize(18, 18);

        this->layout()->addWidget(close_button);
        close_button_ = close_button;
        close_button_->hide();

        QObject::connect(close_button_, &QPushButton::clicked, this,
                         &TabButton::closePressed);
    }
    ~TabButton() { delete close_button_; }

   signals:
    void closePressed();

   protected:
    void enterEvent(QEnterEvent*) override { close_button_->show(); };
    void leaveEvent(QEvent*) override { close_button_->hide(); }

   private:
    QPushButton* close_button_;
};