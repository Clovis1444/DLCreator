#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qboxlayout.h>
#include <qhash.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <QMainWindow>

#include "widgets/tabWidget/tabWidget.h"

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
    void createNewDocument();
    void addToolWidget(QWidget *widget);
    void setupToolWidgets();

   public slots:
    void onActionTools();
    void onActionWorkingArea();
    void onActionTabs();
    static void onActionExit();
    void onActionNew();
    void onToolChanged();

   private:
    Ui::MainWindow *ui_;
    TabWidget *tabWidget_;
};
#endif  // MAINWINDOW_H
