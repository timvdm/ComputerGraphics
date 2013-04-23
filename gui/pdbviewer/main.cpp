#include "mainwindow.h"
#include <QApplication>

#include "pdbwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    if (argc == 2)
      w.pdbWidget()->open(argv[1]);

    w.show();
    
    return a.exec();
}
