#include "mainwindow.h"

#include <qaction.h>
#include <qboxlayout.h>
#include <qdebug.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstringview.h>
#include <qwidget.h>

#include "./ui_mainwindow.h"
#include "file/file.h"

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
}

void MainWindow::createNewFile() {
    auto* file{new File{ui_->tabs_scrollArea, ui_->files_widget}};
    files_.push_back(file);

    // Tab
    auto* tabs_layout{qobject_cast<QHBoxLayout*>(
        ui_->tabs_scrollAreaWidgetContents->layout())};
    tabs_layout->insertWidget(tabs_layout->count() - 1, file->tab());

    // Content
    ui_->files_widget->addWidget(file->content());

    ui_->files_widget->setCurrentWidget(file->content());

    QObject::connect(file->tab(), &QPushButton::clicked, this,
                     &MainWindow::onTabClicked);

    QObject::connect(file->tab(), &TabButton::closePressed, this,
                     &MainWindow::onTabClose);
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Slots/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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
        ui_->tabs_scrollArea->show();
    } else {
        ui_->tabs_scrollArea->hide();
    }
}

void MainWindow::onActionExit() { QApplication::quit(); }

void MainWindow::tab1() { ui_->files_widget->setCurrentIndex(0); }

void MainWindow::onActionNew() { createNewFile(); }

void MainWindow::onTabClicked() {
    auto* tab{qobject_cast<QPushButton*>(sender())};

    for (File* i : files_) {
        if (i->tab() == tab) {
            ui_->files_widget->setCurrentWidget(i->content());
            break;
        }
    }
}

// This function will delete File if sender is a TabButton
// and there is a File in files_ associated with the sender.
// Otherwise, this function will do nothing.
void MainWindow::onTabClose() {
    auto* tab{qobject_cast<TabButton*>(sender())};
    // If sender is a TabButton*
    if (tab) {
        for (File* i : files_) {
            // If tab is an item in files_ -> delete TabButton and content
            // widget
            if (i->tab() == tab) {
                files_.removeOne(i);
                delete i;
                break;
            }
        }
    }
}