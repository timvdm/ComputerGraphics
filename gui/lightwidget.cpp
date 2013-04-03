#include "lightwidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>

#include <libgfx/context_traits.h>


struct LightVertexShader
{
  typedef std::tuple<GFX::ColorF> varying_type;
    
  GFX::vec4 exec(const double *attributes, varying_type &varying)
  {
    using namespace GFX::ShaderFunctions;

    // position and normal in model coordinates
    GFX::vec4 mcPosition(attributes[0], attributes[1], attributes[2], 1.0);

    //std::get<0>(varying) = GFX::ColorF(attributes[3], attributes[4], attributes[5]);
    std::get<0>(varying) = GFX::ColorF(0.0, 1.0, 0.0);
    // transform the vertex position
    return u_mvp * mcPosition;
  }

  // uniforms
  GFX::mat4 u_mvp; // model-view-projection matrix (transform vertices)
};

struct LightFragmentShader
{
  typedef std::tuple<GFX::ColorF> varying_type;

  GFX::ColorF exec(const varying_type &varying, const std::vector<GFX::Texture> &textures, bool backFace = false)
  {
    return std::get<0>(varying);
  }

};



LightWidget::LightWidget(int width, int height, QWidget *parent) : GfxWidget(width, height, parent)
{
  std::shared_ptr<GFX::Mesh> sphere = GFX::Mesh::sphere(3);
  sphere->triangulate();
  int numTriangles = sphere->faces().size();

  m_attributes = sphere->triangleAttributes(true);
}

void LightWidget::render()
{
  typedef GFX::context_traits<LightVertexShader, LightFragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(context(), program);

  context().clearColorBuffer(GFX::Color::black());
  context().clearZBuffer();

  GFX::mat4 rotateX = GFX::rotationMatrix(-angleX(), 0, 1, 0);
  GFX::mat4 rotateY = GFX::rotationMatrix(angleY(), 1, 0, 0);

  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.0, 10);
  GFX::mat4 view = GFX::lookAtMatrix(0, 0, eyeZ(), 0, 0, 0, 0, 1, 0);

  vertexShader.u_mvp = project * view * rotateX * rotateY;

  renderer.drawTriangles(&m_attributes[0], m_attributes.size(), 6);

  copyColorBufferToImage();
}
