#include <QApplication>

#include "mainwindow.h"
#include "tool.h"
#include "widgets/cell/celllayer.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    initCellLayersFromJson();
    Tool::setTool(Liquid::getLiquid("Water"));

    MainWindow w;
    w.setWindowTitle("Hello, world!");
    w.show();
    return QApplication::exec();
}
