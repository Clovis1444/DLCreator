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
#include "widgets/collapsibleSection.h"
#include "widgets/toolWidget.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      tabWidget_{new TabWidget{this}} {
    ui_->setupUi(this);

    setWindowTitle(Settings::kProgramName);

    // Add TabWidget
    ui_->centralwidget->layout()->addWidget(tabWidget_);
    //

    initConnect();

    setupToolWidgets();

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

    //
    // Change status bar on Tool::toolChanged()
    //
    QObject::connect(Tool::signalSender(), &Tool::toolChanged, this,
                     &MainWindow::onToolChanged);
    // Initial set
    onToolChanged();
}

void MainWindow::createNewDocument() { tabWidget_->createTab(); }

void MainWindow::addToolWidget(QWidget* widget) {
    int widgets_count{ui_->tools_frame->layout()->count()};
    QVBoxLayout* layout{qobject_cast<QVBoxLayout*>(ui_->tools_frame->layout())};

    layout->insertWidget(widgets_count - 1, widget);
}

void MainWindow::setupToolWidgets() {
    auto* parent{ui_->tools_frame};

    // Generic tools
    auto* tools{new CollapsibleSection{"Tools", parent}};
    auto* no_tool{new ToolWidget{"No tool", parent}};
    auto* clear_tool{new ToolWidget{"Clear tool", parent, true}};
    tools->addContent(no_tool);
    tools->addContent(clear_tool);
    addToolWidget(tools);

    // Backgrounds
    auto* backgrounds{new CollapsibleSection{"Backgrounds", parent}};
    for (Background* i : Background::list()) {
        auto* widget{new ToolWidget{i, i->name(), backgrounds}};
        backgrounds->addContent(widget);
    }
    addToolWidget(backgrounds);

    // Liquids
    auto* liquids{new CollapsibleSection{"Liquids", parent}};
    for (Liquid* i : Liquid::list()) {
        auto* widget{new ToolWidget{i, i->name(), liquids}};
        liquids->addContent(widget);
    }
    addToolWidget(liquids);

    // Gazes
    auto* gazes{new CollapsibleSection{"Gazes", parent}};
    for (Gaz* i : Gaz::list()) {
        auto* widget{new ToolWidget{i, i->name(), gazes}};
        gazes->addContent(widget);
    }
    addToolWidget(gazes);
};

void MainWindow::onToolChanged() {
    QString msg{"Tool: "};
    msg.append(Tool::toolName());
    ui_->statusbar->showMessage(msg);
};

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
    tabWidget_->setContentVisible(ui_->actionWorkingArea->isChecked());
}

void MainWindow::onActionTabs() {
    tabWidget_->setTabsVisible(ui_->actionTabs->isChecked());
}

void MainWindow::onActionExit() { QApplication::quit(); }

void MainWindow::onActionNew() { createNewDocument(); }
