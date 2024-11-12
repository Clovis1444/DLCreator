#include "mainWindow.h"

#include <qaction.h>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
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
#include "src/widgets/tabWidget/stackedContentWidget/contentWidget/gridManager.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      tabWidget_{new TabWidget{this}},
      toolWidget_{new ToolWidget{this}} {
    // Setup from .ui file
    ui_->setupUi(this);

    setWindowTitle(Settings::kProgramName);
    setWindowIcon(QIcon{Settings::kProgramIconPath});

    // Add ToolWidget
    ui_->centralwidget->layout()->addWidget(toolWidget_);

    // Add TabWidget
    ui_->centralwidget->layout()->addWidget(tabWidget_);

    // TEST
    // tabWidget_->hide();
    // auto* grid{new GridManager{this}};
    // ui_->centralwidget->layout()->addWidget(grid);
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
    // File -> Save As
    QObject::connect(ui_->actionSaveAs, &QAction::triggered, this,
                     &MainWindow::onActionSaveAs);
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

// TODO(clovis): useless function? -> this function was reimplemented as
// MapSaver::getSaveFilePath()
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

// TODO(clovis): this function and onActionSaveAs() have similar code. REFACTOR?
// If file LOADED -> rewrite.
// If file NEW -> call onActionSaveAs()
void MainWindow::onActionSave() {
    if (tabWidget_->tabs_count() == 0) {
        return;
    }

    QPair<int, int> map_size{tabWidget_->gridSize()};

    // Do nothing if there is no grid
    if (map_size.first == 0 && map_size.second == 0) {
        qInfo() << "There is no grid";
        return;
    }

    // Create saves dir if it is not exists
    QDir saves_dir{Settings::SavesDir()};
    if (!saves_dir.exists()) {
        saves_dir.mkpath(".");
    }

    QString save_file_name{};
    // Generate new file path or use file path from already loaded file
    if (tabWidget_->tabFilePath().isEmpty()) {
        save_file_name = MapSaver::getSaveFilePath(tabWidget_->tabName());
        // What do we have:
        // - Files may be just created or loaded
        // - File name may exists or not
        //
        // LOADED -> MAY REWRITE
        // NEW -> if exists call saveAs()
        if (QFile{save_file_name}.exists()) {
            onActionSaveAs();
            return;
        }
    } else {
        save_file_name = tabWidget_->tabFilePath();
    }

    QFile save_file{save_file_name};

    if (!save_file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to write save file: " << save_file.errorString();
    }

    // Create json
    QJsonDocument json{MapSaver::saveMapToFile(tabWidget_->gridManager())};

    // Push json to the file
    QTextStream file_stream{&save_file};
    file_stream << json.toJson(QJsonDocument::Indented);

    save_file.close();

    // Dont forget to update file_path
    tabWidget_->setTabFilePath(save_file_name);
}

void MainWindow::onActionSaveAs() {
    if (tabWidget_->tabs_count() == 0) {
        return;
    }

    QPair<int, int> map_size{tabWidget_->gridSize()};

    // Do nothing if there is no grid
    if (map_size.first == 0 && map_size.second == 0) {
        qInfo() << "There is no grid";
        return;
    }

    // Create saves dir if it is not exists
    QDir saves_dir{Settings::SavesDir()};
    if (!saves_dir.exists()) {
        saves_dir.mkpath(".");
    }

    QString tab_name{tabWidget_->tabName()};

    // Let user choose the file
    QString save_file_name{QFileDialog::getSaveFileName(
        this, QString{"Save %1 as"}.arg(Settings::kSaveFileExtension),
        MapSaver::getSaveFilePath(tabWidget_->tabName()),
        QString{"dlmap (*%1)"}.arg(Settings::kSaveFileExtension))};

    // Return if user press "cancel"
    if (save_file_name.isEmpty()) {
        return;
    }

    QFile save_file{save_file_name};

    if (!save_file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to write save file: " << save_file.errorString();
    }

    // Create json
    QJsonDocument json{MapSaver::saveMapToFile(tabWidget_->gridManager())};

    // Push json to the file
    QTextStream file_stream{&save_file};
    file_stream << json.toJson(QJsonDocument::Indented);

    save_file.close();

    // Dont forget to update file_path and rename tab
    tabWidget_->setTabFilePath(save_file_name);
    tabWidget_->setTabName(QFileInfo{save_file_name}.baseName());
}

// If file already loaded - switches to corresponding file tab
void MainWindow::onActionLoad() {
    // Create saves dir if it is not exists
    QDir saves_dir{Settings::SavesDir()};
    if (!saves_dir.exists()) {
        saves_dir.mkpath(".");
    }

    // Let user choose the file
    QString file_name{QFileDialog::getOpenFileName(
        this, QString{"Load %1"}.arg(Settings::kSaveFileExtension),
        QString{"%1%2%3"}.arg(Settings::SavesDir()),
        QString{"dlmap (*%1)"}.arg(Settings::kSaveFileExtension))};

    // If file already loaded -> switch to corresponding tab and return
    if (tabWidget_->switchToFileTabIfLoaded(file_name)) {
        qInfo() << file_name << "already loaded";
        return;
    }

    QFile file{file_name};

    // Return if failed to open the file
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open save file: " << file.errorString();
        return;
    }

    // Get file content
    QByteArray file_content{file.readAll()};
    file.close();

    // Load map and then create a new tab
    auto* gm{MapSaver::loadMapFromFile(file_name, file_content)};
    // Check if json was loaded correctly
    if (gm == nullptr) return;
    tabWidget_->createTab(gm, QFileInfo{file_name}.baseName(), file_name);
}
