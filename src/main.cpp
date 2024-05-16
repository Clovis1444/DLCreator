#include <QApplication>

#include "mainwindow.h"
#include "widgets/contentwidget.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Cell::init_resources();

    MainWindow w;
    w.setWindowTitle("Hello, world!");
    w.show();
    return QApplication::exec();
}
