#include <QtGui>

#include "gfxwidget.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QPixmap pm(500, 500);

  GfxWidget widget;
  widget.setPixmap(pm);
  widget.show();


  return app.exec();
}
