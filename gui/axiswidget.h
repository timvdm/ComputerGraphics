#ifndef GFX_AXISWIDGET_H
#define GFX_AXISWIDGET_H

#include "gfxwidget.h"

#include <libgfx/context.h>
#include <libgfx/texture.h>

class AxisWidget : public GfxWidget
{
  Q_OBJECT

  public:
    AxisWidget(int width, int height, QWidget *parent = 0);

    void render();

  private:
};

#endif
