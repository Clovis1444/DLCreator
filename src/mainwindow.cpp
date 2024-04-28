#include "mainwindow.h"

#include <qboxlayout.h>
#include <qdebug.h>
#include <qlabel.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstringview.h>
#include <qwidget.h>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);

    initConnect();
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::initConnect() {
    //
    // File menu
    //
    // File -> New
    QObject::connect(ui_->actionNew, &QAction::triggered, this,
                     &MainWindow::onActionNew);
    // File -> Exit
    QObject::connect(ui_->actionExit, &QAction::triggered, this,
                     &MainWindow::onActionExit);

    //
    // View menu
    //
    // View -> Tools
    QObject::connect(ui_->actionTools, &QAction::toggled, this,
                     &MainWindow::onActionTools);
    // View -> Working area
    QObject::connect(ui_->actionWorkingArea, &QAction::toggled, this,
                     &MainWindow::onActionWorkingArea);
    //  View -> Tabs
    QObject::connect(ui_->actionTabs, &QAction::toggled, this,
                     &MainWindow::onActionTabs);

    //
    // Tabs
    //
    QObject::connect(ui_->tabButton1, &QPushButton::clicked, this,
                     &MainWindow::tab1);
    QObject::connect(ui_->tabButton2, &QPushButton::clicked, this,
                     &MainWindow::tab2);
}

void MainWindow::onActionTools() {
    if (ui_->actionTools->isChecked()) {
        ui_->tools_frame->show();
    } else {
        ui_->tools_frame->hide();
    }
}

void MainWindow::onActionWorkingArea() {
    if (ui_->actionWorkingArea->isChecked()) {
        ui_->workingArea_frame->show();
    } else {
        ui_->workingArea_frame->hide();
    }
}

void MainWindow::onActionTabs() {
    if (ui_->actionTabs->isChecked()) {
        ui_->tabs_frame->show();
    } else {
        ui_->tabs_frame->hide();
    }
}

void MainWindow::onActionExit() { QApplication::quit(); }

void MainWindow::tab1() { ui_->files_widget->setCurrentIndex(0); }
void MainWindow::tab2() { ui_->files_widget->setCurrentIndex(1); }

void MainWindow::onActionNew() {
    QString name{"Tab"};
    addNewTab(name);
}

// TODO(clovis): create class "File" with fields: QWidget*, QPushButton*
void MainWindow::addNewTab(QString& name) {
    int tab_index{ui_->files_widget->count()};
    name.append(QString::number(tab_index + 1));

    // Tab button
    auto* tab_button{new QPushButton(name, ui_->tabs_frame)};
    tab_button->setObjectName(name);
    qobject_cast<QBoxLayout*>(ui_->tabs_frame->layout())
        ->insertWidget(tab_index, tab_button);

    // Tab widget
    auto* widget{new QWidget{ui_->files_widget}};
    auto* widget_layout{new QBoxLayout{QBoxLayout::LeftToRight, widget}};
    widget->setLayout(widget_layout);
    auto* label{new QLabel{name}};
    widget->layout()->addWidget(label);

    ui_->files_widget->addWidget(widget);

    QObject::connect(tab_button, &QPushButton::clicked, this, &MainWindow::tab);
}

void MainWindow::tab() {
    QString prefix{"Tab"};
    int tab_index{sender()->objectName().mid(prefix.length()).toInt() - 1};
    ui_->files_widget->setCurrentIndex(tab_index);
}