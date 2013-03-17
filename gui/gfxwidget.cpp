#include "gfxwidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>



GfxWidget::GfxWidget(QWidget *parent) : QLabel(parent), m_image(500, 500, QImage::Format_RGB32),
    m_context(500, 500), m_eye(GFX::vec3(0, 0, -1)), m_dragging(false), m_eyeZ(5.0)
{
  angleX = angleY = 0.0;
}

void GfxWidget::mouseMoveEvent(QMouseEvent *event)
{
  qDebug() << "mouseMoveEvent";

  if (!m_dragging)
    return;
  
  int dx = m_mousePos.x - event->x();
  int dy = m_mousePos.y - event->y();
  //m_eye.theta += dy * 1e-3;
  //m_eye.phi += dx * 1e-3;
  
  angleX += dx * 5e-3;
  angleY += dy * 5e-3;

  update();

  m_mousePos = GFX::Point2D(event->x(), event->y());
}

void GfxWidget::mousePressEvent(QMouseEvent *event)
{
  qDebug() << "mousePressEvent";

  m_dragging = true;
  m_mousePos = GFX::Point2D(event->x(), event->y());
}

void GfxWidget::mouseReleaseEvent(QMouseEvent *event)
{
  qDebug() << "mouseReleaseEvent";
  
  m_dragging = false;
  
  int dx = m_mousePos.x - event->x();
  int dy = m_mousePos.y - event->y();
  //m_eye.theta += dy * 1e-3;
  //m_eye.phi += dx * 1e-3;
  
  angleX += dx * 5e-3;
  angleY += dy * 5e-3;

  update();
}
    
void GfxWidget::wheelEvent(QWheelEvent *event)
{
  qDebug() << "delta = " << event->delta();
  m_eyeZ -= 0.001 * event->delta();
  update();
}
    
void GfxWidget::paintEvent(QPaintEvent *painter)
{
  render();
}

struct SimpleVertexShader
{
  typedef std::tuple<GFX::Color> varying_type;

  GFX::vec4 exec(const double *attributes, varying_type &varying)
  {
    std::get<0>(varying) = GFX::Color(attributes[3], attributes[4], attributes[5]);
    // transform the vertex
    GFX::vec4 position(attributes[0], attributes[1], attributes[2], 1.0);
    return transform * position;
  }

  GFX::mat4 transform;
};

struct SimpleFragmentShader
{
  typedef std::tuple<GFX::Color> varying_type;

  GFX::Color exec(const varying_type &varying, bool backFace = false)
  {
    if (backFace)
      return GFX::Color(200, 200, 200);
    return std::get<0>(varying);
  }
};

double lines[] = { 
                   -0.5,  0.5, -0.5, 255, 255, 255,
                    0.5,  0.5, -0.5, 255, 255, 255,
                    0.5,  0.5, -0.5, 255, 255, 255,
                    0.5, -0.5, -0.5, 255, 255, 255,
                    0.5, -0.5, -0.5, 255, 255, 255,
                   -0.5, -0.5, -0.5, 255, 255, 255,
                   -0.5, -0.5, -0.5, 255, 255, 255,
                   -0.5,  0.5, -0.5, 255, 255, 255,

                   -0.5,  0.5,  0.5, 255, 255, 255,
                    0.5,  0.5,  0.5, 255, 255, 255,
                    0.5,  0.5,  0.5, 255, 255, 255,
                    0.5, -0.5,  0.5, 255, 255, 255,
                    0.5, -0.5,  0.5, 255, 255, 255,
                   -0.5, -0.5,  0.5, 255, 255, 255,
                   -0.5, -0.5,  0.5, 255, 255, 255,
                   -0.5,  0.5,  0.5, 255, 255, 255,

                   -0.5,  0.5, -0.5, 255, 255, 255,
                   -0.5,  0.5,  0.5, 255, 255, 255,
                    0.5,  0.5, -0.5, 255, 255, 255,
                    0.5,  0.5,  0.5, 255, 255, 255,
                    0.5, -0.5, -0.5, 255, 255, 255,
                    0.5, -0.5,  0.5, 255, 255, 255,
                   -0.5, -0.5, -0.5, 255, 255, 255,
                   -0.5, -0.5,  0.5, 255, 255, 255,

                   -0.8, -0.8, -0.8, 255,   0,   0,
                   -0.6, -0.8, -0.8, 255,   0,   0,
                   -0.8, -0.8, -0.8,   0, 255,   0,
                   -0.8, -0.6, -0.8,   0, 255,   0,
                   -0.8, -0.8, -0.8,   0,   0, 255,
                   -0.8, -0.8, -0.6,   0,   0, 255,
};


void GfxWidget::render()
{
  m_fps.startRender();
  m_context.colorBuffer().clear(GFX::Color::black());
  renderCubeLight();
  m_fps.stopRender();
  qDebug() << "FPS: " << m_fps.fps();
}

void GfxWidget::render1()
{
  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(m_context, program);

  GFX::mat4 rotateX = GFX::rotationMatrix(-angleX, 0, 1, 0);
  GFX::mat4 rotateY = GFX::rotationMatrix(angleY, 1, 0, 0);

  GFX::mat4 view = GFX::lookAtMatrix(0, 0, m_eyeZ, 0, 0, 0, 0, 1, 0);
  
  //GFX::mat4 project = GFX::orthoMatrix(-1, 1, -1, 1, 1.0, 20);
  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.0, 20);

  std::cout << "rotateX:" << std::endl << rotateX << std::endl;

  vertexShader.transform = project * view * rotateX * rotateY; 


  std::cout << "transform matrix:" << std::endl << vertexShader.transform << std::endl;

  m_context.zBuffer().clear(std::numeric_limits<double>::max());
  //m_context.disable(GFX::Context::GFX_ZBUFFER);

  renderer.drawLines(lines, 6 * 2 * 15, 6);

  double triangles[] = {
    -0.5, -0.5,  0.0, 255,   0,   0,
     0.5,  0.5,  0.0, 255,   0,   0,
     0.5, -0.5,  0.0, 255,   0,   0,

    -0.5, -0.5,  0.0, 255,   0,   0,
    -0.5,  0.5,  0.0, 255,   0,   0,
     0.5,  0.5,  0.0, 255,   0,   0,

    -0.5,  0.0, -0.5,   0, 255,   0,
     0.5,  0.0,  0.5,   0, 255,   0,
     0.5,  0.0, -0.5,   0, 255,   0,

    -0.5,  0.0, -0.5,   0, 255,   0,
    -0.5,  0.0,  0.5,   0, 255,   0,
     0.5,  0.0,  0.5,   0, 255,   0,

     0.0, -0.5, -0.5,    0,   0, 255,
     0.0,  0.5, -0.5,    0,   0, 255,
     0.0,  0.5,  0.5,    0,   0, 255,

     0.0, -0.5, -0.5,    0,   0, 255,
     0.0,  0.5,  0.5,    0,   0, 255,
     0.0, -0.5,  0.5,    0,   0, 255,

  };

  renderer.drawTriangles(triangles, 3 * 36, 6);


  for (int i = 0; i < m_context.width(); ++i)
    for (int j = 0; j < m_context.height(); ++j) {
      const GFX::Color &color = m_context.colorBuffer()(i, j);
      m_image.setPixel(i, m_context.height() - j - 1, color.toARGB());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);

}

void GfxWidget::render2()
{
  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(m_context, program);

  GFX::mat4 scale = GFX::scaleMatrix(1.0);

  GFX::mat4 rotateX = GFX::rotationMatrix(-angleX, 0, 1, 0);
  GFX::mat4 rotateY = GFX::rotationMatrix(angleY, 1, 0, 0);


  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.0, 10);
  GFX::mat4 view = GFX::lookAtMatrix(0, 0, m_eyeZ, 0, 0, 0, 0, 1, 0);

  vertexShader.transform = project * view * rotateX * rotateY; 


  std::cout << "transform matrix:" << std::endl << vertexShader.transform << std::endl;

  m_context.zBuffer().clear(std::numeric_limits<double>::max());
  //m_context.disable(GFX::Context::GFX_ZBUFFER);

  double triangles[] = {
     // front
    -1,  1, -1, 255,   0,   0, // d
     1, -1, -1,   0, 255,   0, // b
    -1, -1, -1,   0,   0, 255, // a
    -1,  1, -1, 255,   0,   0, // d
     1,  1, -1, 255, 255,   0, // c
     1, -1, -1,   0, 255,   0, // b
     // right
     1,  1, -1, 255,   0,   0, // c
     1, -1,  1, 255,   0,   0, // f
     1, -1, -1, 255,   0,   0, // b
     1,  1, -1, 255,   0,   0, // c
     1,  1,  1, 255,   0,   0, // g
     1, -1,  1, 255,   0,   0, // f
     // back
     1,  1,  1,   0, 255,   0, // g
    -1, -1,  1,   0, 255,   0, // e
     1, -1,  1,   0, 255,   0, // f
     1,  1,  1,   0, 255,   0, // g
    -1,  1,  1,   0, 255,   0, // h
    -1, -1,  1,   0, 255,   0, // e
     // left
    -1,  1,  1,   0,   0, 255, // h
    -1, -1, -1,   0,   0, 255, // a
    -1, -1,  1,   0,   0, 255, // e
    -1,  1,  1,   0,   0, 255, // h
    -1,  1, -1,   0,   0, 255, // d
    -1, -1, -1,   0,   0, 255, // a
     // top
    -1,  1,  1, 255, 255,   0, // h
     1,  1, -1, 255, 255,   0, // c
    -1,  1, -1, 255, 255,   0, // d
    -1,  1,  1, 255, 255,   0, // h
     1,  1,  1, 255, 255,   0, // g
     1,  1, -1, 255, 255,   0, // c
     // bottom
    -1, -1, -1, 255,   0, 255, // a
     1, -1,  1, 255,   0, 255, // f
    -1, -1,  1, 255,   0, 255, // e
    -1, -1, -1, 255,   0, 255, // a
     1, -1, -1, 255,   0, 255, // b
     1, -1,  1, 255,   0, 255, // f
  };


  renderer.drawTriangles(triangles, 36 * 6, 6);


  for (int i = 0; i < m_context.width(); ++i)
    for (int j = 0; j < m_context.height(); ++j) {
      const GFX::Color &color = m_context.colorBuffer()(i, j);
      //m_image.setPixel(i, m_context.height() - j - 1, QColor(color.r, color.g, color.b).rgb());
      m_image.setPixel(i, m_context.height() - j - 1, color.toARGB());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);

}








void GfxWidget::renderCube()
{
  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(m_context, program);

  GFX::mat4 view = GFX::lookAtMatrix(0, 0, 5, 0, 0, 0, 0, 1, 0);
  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.5, 20);

  GFX::mat4 scale = GFX::scaleMatrix(1, 2, 1);

  vertexShader.transform = project * view * scale; 

  m_context.zBuffer().clear(std::numeric_limits<double>::max());

  renderer.drawLines(lines, 6 * 2 * 15, 6);

  for (int i = 0; i < m_context.width(); ++i)
    for (int j = 0; j < m_context.height(); ++j) {
      const GFX::Color &color = m_context.colorBuffer()(i, j);
      m_image.setPixel(i, m_context.height() - j - 1, color.toARGB());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);

}

void GfxWidget::renderHouse()
{
  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(m_context, program);

  GFX::mat4 rotateX = GFX::rotationMatrix(-angleX, 0, 1, 0);
  GFX::mat4 rotateY = GFX::rotationMatrix(angleY, 1, 0, 0);

  GFX::mat4 view = GFX::lookAtMatrix(0, 5, 3 * m_eyeZ, 0, 0, 0, 0, 1, 0);
  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.5, 20);

  vertexShader.transform = project * view * rotateX * rotateY; 

  m_context.zBuffer().clear(std::numeric_limits<double>::max());

  double h = 0.7;

  double quads[] = {
    -0.5,  0.0,  0.5, 50, 100, 255,
     0.5,  0.0,  0.5, 50, 100, 255,
     0.5,    h,  0.5, 50, 100, 255,
    -0.5,    h,  0.5, 50, 100, 255,

    -0.5,  0.0, -0.5, 50, 100, 255,
    -0.5,  0.0,  0.5, 50, 100, 255,
    -0.5,    h,  0.5, 50, 100, 255,
    -0.5,    h, -0.5, 50, 100, 255,

     0.5,  0.0, -0.5, 50, 100, 255,
     0.5,  0.0,  0.5, 50, 100, 255,
     0.5,    h,  0.5, 50, 100, 255,
     0.5,    h, -0.5, 50, 100, 255,

    -0.5,  0.0, -0.5, 50, 100, 255,
     0.5,  0.0, -0.5, 50, 100, 255,
     0.5,    h, -0.5, 50, 100, 255,
    -0.5,    h, -0.5, 50, 100, 255,

    -0.5,        h,  0.5, 255, 0, 0,
     0.0,  0.5 + h,  0.5, 255, 0, 0,
     0.0,  0.5 + h, -0.5, 255, 0, 0,
    -0.5,        h, -0.5, 255, 0, 0,

     0.5,        h,  0.5, 255, 0, 0,
     0.0,  0.5 + h,  0.5, 255, 0, 0,
     0.0,  0.5 + h, -0.5, 255, 0, 0,
     0.5,        h, -0.5, 255, 0, 0,

     4.5,  0.0, -4.5, 100, 100, 100,
     4.5,  0.0,  4.5, 100, 100, 100,
    -4.5,  0.0,  4.5, 100, 100, 100,
    -4.5,  0.0, -4.5, 100, 100, 100
  };


  double trias[] = {
    -0.5,        h,  0.5, 50, 100, 255,
     0.5,        h,  0.5, 50, 100, 255,
     0.0,  0.5 + h,  0.5, 50, 100, 255,
  
     0.5,        h, -0.5, 50, 100, 255,
    -0.5,        h, -0.5, 50, 100, 255,
     0.0,  0.5 + h, -0.5, 50, 100, 255
   };

  
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j) {
      GFX::mat4 save = vertexShader.transform;
      vertexShader.transform *= GFX::translationMatrix(2 * i - 3.5, 0, 2 * j - 3.5);
      renderer.drawQuads(quads, 24 * 6, 6);
      renderer.drawTriangles(trias, 18 * 2, 6);
      vertexShader.transform = save;
    }
      
  renderer.drawQuads(quads + 24 * 6, 24, 6);

  for (int i = 0; i < m_context.width(); ++i)
    for (int j = 0; j < m_context.height(); ++j) {
      const GFX::Color &color = m_context.colorBuffer()(i, j);
      m_image.setPixel(i, j, color.toARGB());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);

}










// 
// Uniform:
//
//   transform (mat4)
//
// Attribute:
// 
//   0-2: vertex xyz
//   3-5: normal xyz
//   6-9: color rgba
//
// Varying:
//
//   0: normal (vec4)
//   1: color (Color)
//

struct LightVertexShader
{
  typedef std::tuple<GFX::vec4, GFX::Color> varying_type;

  GFX::vec4 exec(const double *attributes, varying_type &varying)
  {
    // normal
    std::get<0>(varying) = GFX::vec4(attributes[3], attributes[4], attributes[5], 0.0);
    // color
    std::get<1>(varying) = GFX::Color(attributes[6], attributes[7], attributes[8], attributes[9]);
    // transform the vertex position
    GFX::vec4 position(attributes[0], attributes[1], attributes[2], 1.0);
    return mvp * position;
  }

  GFX::mat4 mv; // model-view matrix (transform normals)
  GFX::mat4 mvp; // model-view-projection matrix (transform vertices)
};

struct LightFragmentShader
{
  typedef std::tuple<GFX::vec4, GFX::Color> varying_type;

  GFX::Color exec(const varying_type &varying, bool backFace = false)
  {
    if (backFace)
      return GFX::Color(200, 200, 200);
    return std::get<1>(varying);
  }
};

namespace GFX {

  template<typename VertexShaderType, typename FragmentShaderType>
  struct context_traits
  {
    typedef VertexShaderType vertex_shader_type;
    typedef FragmentShaderType fragment_shader_type;
    typedef typename vertex_shader_type::varying_type varying_type;
    typedef Program<vertex_shader_type, fragment_shader_type> program_type;
    typedef Renderer<program_type> renderer_type;
  };

}

void GfxWidget::renderCubeLight()
{
  typedef GFX::context_traits<LightVertexShader, LightFragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(m_context, program);

  m_context.clearZBuffer();

  GFX::mat4 rotateX = GFX::rotationMatrix(-angleX, 0, 1, 0);
  GFX::mat4 rotateY = GFX::rotationMatrix(angleY, 1, 0, 0);

  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.0, 10);
  GFX::mat4 view = GFX::lookAtMatrix(0, 0, m_eyeZ, 0, 0, 0, 0, 1, 0);

  vertexShader.mvp = project * view * rotateX * rotateY; 

  // Get cube attributes: vertex xyz, normal xyz, color rgba (10 doubles per vertex)
  std::shared_ptr<GFX::Mesh> cube = GFX::Mesh::cube();
  std::vector<double> attributes = cube->quadAttributes(true, true);

  /*
  std::cout << "Attributes: " << attributes.size() << std::endl;
  for (int i = 0; i < attributes.size() / 10; ++i) {
    for (int j = 0; j < 10; ++j)
      std::cout << attributes[10 * i + j] << " ";
    std::cout << std::endl;
  }
  */


  renderer.drawQuads(&attributes[0], attributes.size(), 10);

  for (int i = 0; i < m_context.width(); ++i)
    for (int j = 0; j < m_context.height(); ++j) {
      const GFX::Color &color = m_context.colorBuffer()(i, j);
      m_image.setPixel(i, m_context.height() - j - 1, color.toARGB());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);
}

