#include "axiswidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>

#include <libgfx/context_traits.h>

using namespace GFX;

struct VertexShader
{
  typedef std::tuple<ColorF> varying_type;
    
  vec4 exec(const double *attributes, varying_type &varying)
  {
    std::get<0>(varying) = ColorF(attributes[3], attributes[4], attributes[5]);
    return u_mvp * vec4(attributes[0], attributes[1], attributes[2], 1.0);
  }

  // uniforms
  mat4 u_mvp; // model-view-projection matrix (transform vertices)
};

struct FragmentShader
{
  typedef std::tuple<ColorF> varying_type;

  ColorF exec(const varying_type &varying, const std::vector<Texture> &textures, vec3 &pos, bool backFace = false)
  {
    return std::get<0>(varying);
  }

};





AxisWidget::AxisWidget(int width, int height, QWidget *parent) : GfxWidget(width, height, parent)
{
}

void AxisWidget::render()
{
  typedef context_traits<VertexShader, FragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(context(), program);

  context().clearColorBuffer(Color::black());
  context().clearZBuffer();

  mat4 rotateX = rotationMatrix(-angleX(), 0, 1, 0);
  mat4 rotateY = rotationMatrix(angleY(), 1, 0, 0);

  mat4 project = frustumMatrix(-1, 1, -1, 1, 1.0, 10);
  mat4 view = lookAtMatrix(0, 0, 3, 0, 0, 0, 0, 1, 0); // view along reverse z-axis

  vertexShader.u_mvp = project * view * rotateX * rotateY;

  double axis[] = {
    -1, -1, -1, 1, 0, 0,
     1, -1, -1, 1, 0, 0,
    -1, -1, -1, 0, 1, 0,
    -1,  1, -1, 0, 1, 0,
    -1, -1, -1, 0, 0, 1,
    -1, -1,  1, 0, 0, 1
  };

  renderer.drawLines(axis, 36, 6);



    
  copyColorBufferToImage();
}



