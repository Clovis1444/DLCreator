#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void addNewTab(QString &name);

   public slots:
    void onActionTools();
    void onActionWorkingArea();
    void onActionTabs();
    static void onActionExit();
    void tab1();
    void tab2();
    void tab();
    void onActionNew();

   private:
    Ui::MainWindow *ui_;
};
#endif  // MAINWINDOW_H
