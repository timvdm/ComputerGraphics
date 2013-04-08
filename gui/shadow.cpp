#include <QtGui>

#include "shadowwidget.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  ShadowWidget widget(500, 500);
  widget.show();

  return app.exec();
}
