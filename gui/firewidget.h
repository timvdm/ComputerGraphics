#ifndef GFX_LIGHTWIDGET_H
#define GFX_LIGHTWIDGET_H

#include "gfxwidget.h"

#include <libgfx/context.h>
#include <libgfx/texture.h>

struct Particle
{
  GFX::vec3 pos;
  GFX::vec3 speed;
  GFX::ColorF color;
  GFX::ColorF deltaColor;
  GFX::Real origLifetime;
  GFX::Real lifetime;
  GFX::Real radius;
};
  
class ParticleSystem
{
  public:
    ParticleSystem(const GFX::vec3 &pos, const GFX::vec3 &posVar,
                   const GFX::vec3 &speed, const GFX::vec3 &speedVar,
                   const GFX::ColorF &beginColor, const GFX::ColorF &beginColorVar,
                   const GFX::ColorF &endColor, const GFX::ColorF &endColorVar,
                   GFX::Real radius, GFX::Real radiusVar,
                   GFX::Real lifetime, GFX::Real lifetimeVar,
                   int maxParticles = 10000);

    void setEmitInterval(GFX::Real interval)
    {
      m_emitInterval = interval;
    }

    void emitParticle();
    void update(GFX::Real dt);

    int numParticles() const
    {
      return m_numParticles;
    }

    const Particle& particle(int i) const
    {
      return m_particles[i];
    }

  private:
    GFX::vec3 m_pos;
    GFX::vec3 m_posVar;
    GFX::vec3 m_speed;
    GFX::vec3 m_speedVar;
    GFX::ColorF m_beginColor;
    GFX::ColorF m_beginColorVar;
    GFX::ColorF m_endColor;
    GFX::ColorF m_endColorVar;
    GFX::Real m_radius;
    GFX::Real m_radiusVar;
    GFX::Real m_lifetime;
    GFX::Real m_lifetimeVar;


    std::vector<Particle> m_particles;
    int m_numParticles;
    GFX::Real m_emitInterval; 
    GFX::Real m_elapsed; // elapsed since last emit
};

class FireWidget : public GfxWidget
{
  Q_OBJECT

  public:
    FireWidget(int width, int height, QWidget *parent = 0);

  public slots:

    void render();


  private:
    ParticleSystem m_particleSystem;
    QTimer m_timer;

};

#endif
