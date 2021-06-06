#include <QApplication>
#include "man.h"
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget s;
    s.show();
    return a.exec();
}
