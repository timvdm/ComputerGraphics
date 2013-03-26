#include <QtGui>

#include "texturewidget.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  TextureWidget widget(800, 800);
  widget.show();

  return app.exec();
}
