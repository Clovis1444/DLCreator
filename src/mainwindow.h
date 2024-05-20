#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qhash.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <QMainWindow>

#include "file.h"

class File;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private:
    void initConnect();
    void createNewFile();
    void addToolWidget(QWidget *widget);
    void setupToolWidgets();

   public slots:
    void onActionTools();
    void onActionWorkingArea();
    void onActionTabs();
    static void onActionExit();
    void onActionNew();
    void onTabClicked();
    void onTabClose();

   private:
    Ui::MainWindow *ui_;
    QList<File *> files_;
};
#endif  // MAINWINDOW_H
