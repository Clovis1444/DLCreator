#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);

    init_connect();
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::init_connect() {
    //
    // File menu
    //
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
