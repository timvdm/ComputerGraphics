#ifndef GFX_LIGHTWIDGET_H
#define GFX_LIGHTWIDGET_H

#include "gfxwidget.h"

#include <libgfx/context.h>
#include <libgfx/texture.h>

class PDBWidget : public GfxWidget
{
  Q_OBJECT

  public:
    PDBWidget(QWidget *parent = 0);

    void open(const std::string &filename);

    void render();

    void setZeta(GFX::Real zeta)
    {
      m_zeta = zeta;
    }

    void setEta(GFX::Real eta)
    {
      m_eta = eta;
    }

    void setNumRays(int numRays)
    {
      m_numRays = numRays;
    }

    void setAOMapSize(int aoMapSize)
    {
      m_aoMapSize = aoMapSize;
    }

    void setAOOnly(bool value)
    {
      m_aoOnly = value;
    }

    void enableAO(bool value)
    {
      m_aoEnabled = value;
    }

    void computeAmbientOcclusion();

    const GFX::Buffer<GFX::ColorF>& aoMap() const
    {
      return m_aoMap;
    }

  private:
    GFX::Buffer<GFX::Real> createDepthMap(const GFX::vec3 &dir);
    void createAmbientOcclusionMap(const GFX::vec3 &dir);

    std::vector<double> m_attributes;
    std::vector<GFX::vec4> m_colors;
    GFX::Buffer<GFX::ColorF> m_aoMap;

    GFX::Real m_zeta; // zeta for contours
    GFX::Real m_eta; // eta for contours
    int m_numRays; // number of rays for ambient occlusion
    int m_aoMapSize; // size for ambient occlusion maps
    bool m_aoOnly;
    bool m_aoEnabled;
};

#endif
