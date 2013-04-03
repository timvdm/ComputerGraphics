#ifndef GFX_LIGHTWIDGET_H
#define GFX_LIGHTWIDGET_H

#include "gfxwidget.h"

#include <libgfx/context.h>
#include <libgfx/texture.h>

class LightWidget : public GfxWidget
{
  Q_OBJECT

  public:
    LightWidget(int width, int height, QWidget *parent = 0);

    void render();

  private:
    std::vector<double> m_attributes;
};

#endif
