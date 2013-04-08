#ifndef GFX_SHADOWWIDGET_H
#define GFX_SHADOWWIDGET_H

#include "gfxwidget.h"

#include <libgfx/context.h>
#include <libgfx/buffer.h>

class ShadowWidget : public GfxWidget
{
  Q_OBJECT

  public:
    ShadowWidget(int width, int height, QWidget *parent = 0);

    void render();
    void createShadowMap();

  private:
    GFX::Buffer<GFX::Color> m_shadowMap;
};

#endif
