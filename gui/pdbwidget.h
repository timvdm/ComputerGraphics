#ifndef GFX_LIGHTWIDGET_H
#define GFX_LIGHTWIDGET_H

#include "gfxwidget.h"

#include <libgfx/context.h>
#include <libgfx/texture.h>

class PDBWidget : public GfxWidget
{
  Q_OBJECT

  public:
    PDBWidget(const std::string &filename, int width, int height, QWidget *parent = 0);

    void render();

  private:
    GFX::Buffer<GFX::Real> createDepthMap(const GFX::vec3 &dir);
    void createAmbientOcclusionMap(const GFX::vec3 &dir);

    std::vector<double> m_attributes;
    std::vector<GFX::vec4> m_colors;
    GFX::Buffer<GFX::Color> m_aoMap;
};

#endif
