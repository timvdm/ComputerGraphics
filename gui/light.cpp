#include <QtGui>

#include "lightwidget.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  LightWidget widget(500, 500);
  widget.show();

  return app.exec();
}
