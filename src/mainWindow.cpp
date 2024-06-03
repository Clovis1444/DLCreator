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
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);

    setWindowTitle(Settings::kProgramName);

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

void MainWindow::createNewDocument() {
    auto* doc{new Document{ui_->tabs_scrollArea, ui_->documents_widget}};
    documents_.push_back(doc);

    // Tab
    auto* tabs_layout{qobject_cast<QHBoxLayout*>(
        ui_->tabs_scrollAreaWidgetContents->layout())};
    tabs_layout->insertWidget(tabs_layout->count() - 1, doc->tab());

    // Content
    ui_->documents_widget->addWidget(doc->content());

    ui_->documents_widget->setCurrentWidget(doc->content());

    QObject::connect(doc->tab(), &QPushButton::clicked, this,
                     &MainWindow::onTabClicked);

    QObject::connect(doc->tab(), &TabButton::closePressed, this,
                     &MainWindow::onTabClose);
}

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

void MainWindow::onActionNew() { createNewDocument(); }

// TODO(clovis): implement change active tab color
void MainWindow::onTabClicked() {
    auto* tab{qobject_cast<TabButton*>(sender())};

    for (auto* i : documents_) {
        if (i->tab() == tab) {
            ui_->documents_widget->setCurrentWidget(i->content());
            break;
        }
    }
}

// This function will delete File if sender is a TabButton
// and there is a File in documents_ associated with the sender.
// Otherwise, this function will do nothing.
void MainWindow::onTabClose() {
    auto* tab{qobject_cast<TabButton*>(sender())};
    // If sender is a TabButton*
    if (tab) {
        for (auto* i : documents_) {
            // If tab is an item in documents_ -> delete TabButton and content
            // widget
            if (i->tab() == tab) {
                documents_.removeOne(i);
                delete i;
                break;
            }
        }
    }
}