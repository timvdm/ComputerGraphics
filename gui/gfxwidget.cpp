#include "gfxwidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>



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
  render2();
  m_fps.stopRender();
  qDebug() << "FPS: " << m_fps.fps();
}

void GfxWidget::render1()
{
  GFX::Context context(pixmap()->width(), pixmap()->height());

  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(context, program);

  GFX::mat4 rotateX = GFX::rotationMatrix(-angleX, 0, 1, 0);
  GFX::mat4 rotateY = GFX::rotationMatrix(angleY, 1, 0, 0);

  GFX::mat4 view = GFX::lookAtMatrix(0, 0, m_eyeZ, 0, 0, 0, 0, 1, 0);
  
  //GFX::mat4 project = GFX::orthoMatrix(-1, 1, -1, 1, 1.0, 20);
  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.0, 20);

  std::cout << "rotateX:" << std::endl << rotateX << std::endl;

  vertexShader.transform = project * view * rotateX * rotateY; 


  std::cout << "transform matrix:" << std::endl << vertexShader.transform << std::endl;

  context.zBuffer().clear(std::numeric_limits<double>::max());
  //context.disable(GFX::Context::GFX_ZBUFFER);

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


  for (int i = 0; i < context.width(); ++i)
    for (int j = 0; j < context.height(); ++j) {
      const GFX::Color &color = context.colorBuffer()(i, j);
      m_image.setPixel(i, m_context.height() - j - 1, color.toARGB());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);

}

void GfxWidget::render2()
{
  //GFX::Context context(pixmap()->width(), pixmap()->height());

  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(m_context, program);

  GFX::mat4 scale = GFX::scaleMatrix(1.0);

  //GFX::mat4 rotateX = GFX::xRotationMatrix(angleY);
  //GFX::mat4 rotateY = GFX::yRotationMatrix(angleX);
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
  GFX::Context context(pixmap()->width(), pixmap()->height());

  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(context, program);

  GFX::mat4 view = GFX::lookAtMatrix(0, 0, 5, 0, 0, 0, 0, 1, 0);
  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.5, 20);

  GFX::mat4 scale = GFX::scaleMatrix(1, 2, 1);

  vertexShader.transform = project * view * scale; 

  context.zBuffer().clear(std::numeric_limits<double>::max());

  renderer.drawLines(lines, 6 * 2 * 15, 6);

  for (int i = 0; i < context.width(); ++i)
    for (int j = 0; j < context.height(); ++j) {
      const GFX::Color &color = context.colorBuffer()(i, j);
      m_image.setPixel(i, m_context.height() - j - 1, color.toARGB());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);

}

