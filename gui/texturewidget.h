#ifndef GFX_TEXTUREWIDGET_H
#define GFX_TEXTUREWIDGET_H

#include "gfxwidget.h"

#include <libgfx/context.h>
#include <libgfx/texture.h>

class TextureWidget : public GfxWidget
{
  Q_OBJECT

  public:
    TextureWidget(int width, int height, QWidget *parent = 0);

    void render();

  private:
    std::vector<GFX::Texture> m_textures;
};

#endif
