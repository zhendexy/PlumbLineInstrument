#include "PlumbLineInstrument.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlumbLineInstrument w;
    w.show();
    return a.exec();
}
