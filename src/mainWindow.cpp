#include "mainWindow.h"

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

#include "./ui_mainWindow.h"
#include "settings.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      tabWidget_{new TabWidget{this}},
      toolWidget_{new ToolWidget{this}} {
    // Setup from .ui file
    ui_->setupUi(this);

    setWindowTitle(Settings::kProgramName);

    // Add ToolWidget
    ui_->centralwidget->layout()->addWidget(toolWidget_);

    // Add TabWidget
    ui_->centralwidget->layout()->addWidget(tabWidget_);

    //

    initConnect();

    // setupToolWidgets();

    createNewDocument();
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
    //  View -> History
    QObject::connect(ui_->actionHistory, &QAction::toggled, this,
                     &MainWindow::onActionHistory);

    //
    // Change status bar on Tool::toolChanged()
    //
    QObject::connect(Tool::signalSender(), &Tool::toolChanged, this,
                     &MainWindow::onToolChanged);
    // Initial set
    onToolChanged();
}

void MainWindow::createNewDocument() { tabWidget_->createTab(); }

void MainWindow::onToolChanged() {
    QString msg{"Tool: "};
    msg.append(Tool::toolName());
    ui_->statusbar->showMessage(msg);
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Slots/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void MainWindow::onActionTools() {
    toolWidget_->setVisible(ui_->actionTools->isChecked());
}

void MainWindow::onActionWorkingArea() {
    tabWidget_->setContentVisible(ui_->actionWorkingArea->isChecked());
}

void MainWindow::onActionTabs() {
    tabWidget_->setTabsVisible(ui_->actionTabs->isChecked());
}

void MainWindow::onActionHistory() {
    tabWidget_->setHistoryVisible(ui_->actionHistory->isChecked());
}

void MainWindow::onActionExit() { QApplication::quit(); }

void MainWindow::onActionNew() { createNewDocument(); }
