#include "CppLsnGui.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
    QApplication a(argc, argv);
    CppLsnGui w;
    w.show();
    return a.exec();
}
