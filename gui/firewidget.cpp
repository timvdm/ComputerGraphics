#include "firewidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>

#include <libgfx/context_traits.h>

#include <random>

using namespace GFX;

Real random_float(Real begin, Real end)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(begin, end);
  return dis(gen);
}

Real rand11()
{
  return random_float(-1.0, 1.0);
}

int random_int(int begin, int end)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(begin, end);
  return dis(gen);
}



ParticleSystem::ParticleSystem(const vec3 &pos, const vec3 &posVar,
                               const vec3 &speed, const vec3 &speedVar,
                               const ColorF &beginColor, const ColorF &beginColorVar,
                               const ColorF &endColor, const ColorF &endColorVar,
                               Real radius, Real radiusVar,
                               Real lifetime, Real lifetimeVar,
                               int maxParticles)
    : m_pos(pos), m_posVar(posVar), m_speed(speed), m_speedVar(speedVar),
      m_beginColor(beginColor), m_beginColorVar(beginColorVar),
      m_endColor(endColor), m_endColorVar(endColorVar),
      m_radius(radius), m_radiusVar(radiusVar),
      m_lifetime(lifetime), m_lifetimeVar(lifetimeVar)
{
  m_particles.resize(maxParticles);
  m_numParticles = 0;
  m_emitInterval = 1.0;
  m_elapsed = 0.0;
}

void ParticleSystem::emitParticle()
{
  Particle &p = m_particles[m_numParticles];

  p.pos = m_pos + vec3(m_posVar.x() * rand11(), m_posVar.y() * rand11(), m_posVar.z() * rand11());
  p.speed = m_speed + vec3(m_speedVar.x() * rand11(), m_speedVar.y() * rand11(), m_speedVar.z() * rand11());
  p.lifetime = p.origLifetime = m_lifetime + m_lifetimeVar * rand11();
  p.radius = m_radius + m_radiusVar * rand11();

  ColorF beginColor(m_beginColor.r + m_beginColorVar.r * rand11(),
                    m_beginColor.g + m_beginColorVar.r * rand11(),
                    m_beginColor.b + m_beginColorVar.b * rand11(),
                    m_beginColor.a + m_beginColorVar.a * rand11());
  ColorF endColor(m_endColor.r + m_endColorVar.r * rand11(),
                  m_endColor.g + m_endColorVar.r * rand11(),
                  m_endColor.b + m_endColorVar.b * rand11(),
                  m_endColor.a + m_endColorVar.a * rand11());

  p.color = beginColor;
  p.deltaColor = ColorF((endColor.r - beginColor.r) / p.lifetime,
                        (endColor.g - beginColor.g) / p.lifetime,
                        (endColor.b - beginColor.b) / p.lifetime,
                        (endColor.a - beginColor.a) / p.lifetime);

  ++m_numParticles;
}

void ParticleSystem::update(Real dt)
{
  for (int i = 0; i < m_numParticles; ++i) {
    Particle &p = m_particles[i];
    // update lifetime
    p.lifetime -= dt;

    if (p.lifetime < 0.0) {
      // particle died...
      std::swap(m_particles[i], m_particles[m_numParticles - 1]);
      --m_numParticles;   
    } else {
      // update particle position
      p.pos += p.speed * dt;
      // update color
      p.color = ColorF(p.color.r + p.deltaColor.r * dt,
                       p.color.g + p.deltaColor.g * dt,
                       p.color.b + p.deltaColor.b * dt,
                       p.color.a + p.deltaColor.a * dt);
    }
  }

  m_elapsed += dt;
  if (m_elapsed >= m_emitInterval) {
    int n = m_elapsed / m_emitInterval;
    for (int i = 0; i < n; ++i)
      emitParticle();
    m_elapsed -= n * m_emitInterval;
  }
}

struct FireVertexShader
{
  // s, t, rad, color
  typedef std::tuple<Real, Real, Real, ColorF> varying_type;
    
  vec4 exec(const double *attributes, varying_type &varying)
  {
    using namespace ShaderFunctions;

    // project sphere center using model-view matrix
    vec4 pos = u_mv * vec4(attributes[0], attributes[1], attributes[2], 1.0);

    // get references to varyings
    Real &s = std::get<0>(varying);
    Real &t = std::get<1>(varying);
    Real &r = std::get<2>(varying);

    // assign attribute values to varyings
    s = attributes[3];
    t = attributes[4];
    r = attributes[9];

    std::get<3>(varying) = ColorF(attributes[5], attributes[6], attributes[7], attributes[8]);

    // displace quad vertex
    pos.x() += s * r;
    pos.y() += t * r;

    return u_p * pos;
  }

  // uniforms
  mat4 u_mv; // model-view matrix
  mat4 u_p; // projection matrix
};

struct FireFragmentShader
{
  typedef std::tuple<Real, Real, Real, ColorF> varying_type;

  ColorF exec(const varying_type &varying, const std::vector<Texture> &textures, vec3 &pos, bool backFace = false)
  {
    // get varyings
    Real s = std::get<0>(varying);
    Real t = std::get<1>(varying);
    //Real r = std::get<2>(varying);

    // check to make sure we are inside the sphere
    Real d2 = s * s + t * t;
    if (d2 >= 1.0)
      return Color::none();

    Real d = std::sqrt(d2);

    const ColorF &color = std::get<3>(varying);

    return ColorF(color.r * (1.0 - d), color.g * (1.0 - d), color.b * (1.0 - d), color.a * (1.0 - d));
  }
};



FireWidget::FireWidget(int width, int height, QWidget *parent) : GfxWidget(width, height, parent),
  m_particleSystem(vec3(0, 0, 0), vec3(0.4, 0, 0.4), vec3(0, 0.5, 0), vec3(0, 0.15, 0),
                   ColorF(1, 0, 0, 1), ColorF(0, 0, 0, 0), ColorF(1, 1, 0, 0), ColorF(0, 0, 0, 0),
                   0.1, 0.02, 3.0, 1.0)
{
  setEyeZ(2);

  connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_timer.setInterval(100);
  m_timer.start();

  m_particleSystem.setEmitInterval(0.01);
}

void FireWidget::render()
{
  typedef context_traits<FireVertexShader, FireFragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(context(), program);

  // clear color & Z buffers
  context().clearColorBuffer(Color::black());
  context().clearZBuffer();

  context().disable(GFX_BUFFER, GFX_Z_BUFFER);

  // create transformation matrices
  mat4 rotateX = rotationMatrix(-angleX(), 0, 1, 0);
  mat4 rotateY = rotationMatrix(angleY(), 1, 0, 0);
  Real aspect = static_cast<Real>(context().width()) / context().height();
  mat4 project = frustumMatrix(-aspect, aspect, -1, 1, 1.0, 10);
  mat4 view = lookAtMatrix(0, 0, eyeZ(), 0, 0, 0, 0, 1, 0);

  // set vertex shader's matrices
  vertexShader.u_mv = view * rotateX * rotateY;
  vertexShader.u_p = project;

  // x y z s t r g b a rad
  Real attr[] = {
    0, 0, 0,  1,  1, 0, 0, 0, 0, 0,
    0, 0, 0, -1,  1, 0, 0, 0, 0, 0,
    0, 0, 0, -1, -1, 0, 0, 0, 0, 0,
    0, 0, 0,  1, -1, 0, 0, 0, 0, 0
  };

  for (int i = 0; i < m_particleSystem.numParticles(); ++i) {
    const Particle &p = m_particleSystem.particle(i);
    attr[0] = attr[10] = attr[20] = attr[30] = p.pos.x();
    attr[1] = attr[11] = attr[21] = attr[31] = p.pos.y();
    attr[2] = attr[12] = attr[22] = attr[32] = p.pos.z() + i * 10e-5;

    attr[3] = attr[4] = attr[14] = attr[33] = 1.0;
    attr[13] = attr[23] = attr[24] = attr[34] = -1.0;

    attr[5] = attr[15] = attr[25] = attr[35] = p.color.r;
    attr[6] = attr[16] = attr[26] = attr[36] = p.color.g;
    attr[7] = attr[17] = attr[27] = attr[37] = p.color.b;
    attr[8] = attr[18] = attr[28] = attr[38] = p.color.a;

    attr[9] = attr[19] = attr[29] = attr[39] = p.radius;

    renderer.drawQuads(attr, 40, 10);
  }


  copyColorBufferToImage();
  m_particleSystem.update(0.1);
}
