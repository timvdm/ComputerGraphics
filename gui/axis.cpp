#include <QtGui>

#include "axiswidget.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  AxisWidget widget(500, 500);
  widget.show();

  return app.exec();
}
