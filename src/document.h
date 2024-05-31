#pragma once

#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qlabel.h>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "widgets/contentWidget/contentWidget.h"
#include "widgets/tabButton.h"

class Document {
   public:
    Document(QWidget* tab_parent, QStackedWidget* content_parent,
             const QString& name = "New document")
        : tab_{new TabButton{tab_parent, name}},
          content_{new ContentWidget{content_parent}} {}

    ~Document() {
        delete tab_;
        delete content_;
    }
    TabButton* tab() { return tab_; }
    QWidget* content() { return content_; }

   private:
    TabButton* tab_;
    ContentWidget* content_;
};