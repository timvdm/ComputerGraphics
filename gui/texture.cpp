#include <QtGui>

#include "texturewidget.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  TextureWidget widget(500, 500);
  widget.show();

  return app.exec();
}
