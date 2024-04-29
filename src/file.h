#pragma once

#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qlabel.h>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <qwidget.h>
class File {
   public:
    File(QWidget* tab_parent, QStackedWidget* content_parent,
         const QString& name = "New file")
        : tab_{new QPushButton{name, tab_parent}},
          content_{new QWidget{content_parent}} {
        // Content
        content_->setLayout(new QHBoxLayout{});
        content_->layout()->addWidget(new QLabel{name, content_});
    }

    ~File() {
        delete tab_;
        delete content_;
    }
    QPushButton* tab() { return tab_; }
    QWidget* content() { return content_; }

   private:
    QPushButton* tab_;
    QWidget* content_;
};