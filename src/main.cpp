#include <QApplication>

#include "mainWindow.h"
#include "resources.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    Resources::init_resources();

    MainWindow w;
    w.show();
    return QApplication::exec();
}
