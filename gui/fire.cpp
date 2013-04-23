#include <QtGui>

#include "firewidget.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  FireWidget widget(500, 500);
  widget.show();

  return app.exec();
}
