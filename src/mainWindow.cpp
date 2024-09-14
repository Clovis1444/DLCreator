#include "mainWindow.h"

#include <qaction.h>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstringview.h>
#include <qwidget.h>

#include "./ui_mainWindow.h"
#include "settings.h"
#include "src/mapSaver/mapSaver.h"

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
    // File -> Save
    QObject::connect(ui_->actionSave, &QAction::triggered, this,
                     &MainWindow::onActionSave);
    // File -> Load
    QObject::connect(ui_->actionLoad, &QAction::triggered, this,
                     &MainWindow::onActionLoad);
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

QString MainWindow::getNewSaveFileName(QPair<int, int> map_size) {
    QString file_name{QString::number(map_size.first) + "x" +
                      QString::number(map_size.second) + "map"};
    QFile save_file{Settings::SavesDir() + file_name +
                    Settings::kSaveFileExtension};

    // If file does not exists - return default file name
    if (!save_file.exists()) {
        return save_file.fileName();
    }

    // If file already exists - return file name with number
    int count{1};
    while (true) {
        save_file.setFileName(Settings::SavesDir() + file_name + '(' +
                              QString::number(count) + ')' +
                              Settings::kSaveFileExtension);

        if (!save_file.exists()) {
            return save_file.fileName();
        }

        ++count;
    }
}

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

// TODO(clovis): implement this function using QFileDialog
void MainWindow::onActionSave() {
    QPair<int, int> map_size{tabWidget_->gridSize()};

    // Do nothing if there is no grid
    if (map_size.first == 0 && map_size.second == 0) {
        qDebug() << "There is no grid";
        return;
    }

    // Create saves dir if it is not exists
    QDir saves_dir{Settings::SavesDir()};
    if (!saves_dir.exists()) {
        saves_dir.mkpath(".");
    }

    // Create file
    QFile save_file{getNewSaveFileName(map_size)};

    if (!save_file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to write save file: " << save_file.errorString();
    }

    // Create json
    QJsonDocument json{MapSaver::saveMapToFile(tabWidget_->cellCollection())};

    // Push json to the file
    QTextStream file_stream{&save_file};
    file_stream << json.toJson(QJsonDocument::Indented);

    save_file.close();
}

void MainWindow::onActionLoad() {
    auto file_open_callback = [this](const QString& file_name,
                                     const QByteArray& file_content) {
        if (!file_name.isEmpty()) {
            auto* cc{MapSaver::loadMapFromFile(file_name, file_content)};

            // Check if json was loaded correctly
            if (cc == nullptr) return;

            tabWidget_->createTab(cc, QFileInfo{file_name}.baseName());
        } else {
            qDebug() << "No file was selected";
        }
    };

    QFileDialog::getOpenFileContent(
        QString{"dlmap (*%1)"}.arg(Settings::kSaveFileExtension),
        file_open_callback, this);
}