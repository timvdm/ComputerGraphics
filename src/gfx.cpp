#include "libgfx/context.h"
#include "libgfx/program.h"
#include "libgfx/renderer.h"

#include "utils.h"

#include <fstream>
#include <tuple>

using namespace GFX;

img::EasyImage convert(const Context &context)
{
  img::EasyImage image(context.width(), context.height());

  for (int i = 0; i < context.width(); ++i)
    for (int j = 0; j < context.height(); ++j) {
      const Color &color = context.colorBuffer()(i, j);
      image(i, j) = img::Color(255 * color.r, 255 * color.g, 255 * color.b);
    }

  return image;
}

struct SimpleVertexShader
{
  typedef std::tuple<Color> varying_type;

  vec4 exec(const double *attributes, varying_type &varying)
  {
    std::get<0>(varying) = Color(attributes[3], attributes[4], attributes[5]);
    // transform the vertex
    vec4 position(attributes[0], attributes[1], attributes[2], 1.0);
    return projectionMatrix * position;
  }

  mat4 projectionMatrix;
};

struct SimpleFragmentShader
{
  typedef std::tuple<Color> varying_type;

  Color exec(const varying_type &varying)
  {
    return std::get<0>(varying);
  }
};


int main()
{
  Context context(1024, 768);

  std::ofstream ofs("gfx.bmp", std::ios_base::out | std::ios_base::binary);

  double lines[] = { 100, 100, 0, 1, 0, 0,
                     300, 300, 0, 0, 1, 0,
                     300, 100, 0.1, 0, 0, 1,
                     100, 300, 0.1, 0, 0, 1 };

  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  Renderer<Program<SimpleVertexShader, SimpleFragmentShader> > renderer(context, program);

  vertexShader.projectionMatrix = projectionMatrix(0, 0, -1);

  context.zBuffer().clear(std::numeric_limits<double>::max());
  //context.disable(Context::GFX_ZBUFFER);

  renderer.drawLines(lines, 24, 6);

  double triangles[] = { 400, 400, 0, 1, 0, 0,
                         400, 800, 0, 0, 1, 0,
                         600, 600, 0.3, 0, 0, 1,
                         350, 450, 0.1, 0, 0, 1,
                         350, 550, 0.1, 0, 1, 1,
                         550, 500, 0.1, 0, 1, 0 };

  renderer.drawTriangles(triangles, 36, 6);


  ofs << convert(context);
}
