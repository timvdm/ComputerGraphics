#include "gfxwidget.h"

#include <libgfx/program.h>
#include <libgfx/renderer.h>

GfxWidget::GfxWidget(QWidget *parent) : QLabel(parent), m_image(800, 600, QImage::Format_RGB32),
    m_context(800, 600), m_eye(GFX::vec3(0, 0, -1)), m_dragging(false)
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
  
  angleX += dx * 1e-3;
  angleY += dy * 1e-3;

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
  
  angleX += dx * 1e-3;
  angleY += dy * 1e-3;

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

  GFX::Color exec(const varying_type &varying)
  {
    return std::get<0>(varying);
  }
};

double lines[] = { 
                   -0.5,  0.5, -0.5, 1, 0, 0,
                    0.5,  0.5, -0.5, 0, 1, 0,
                    0.5,  0.5, -0.5, 0, 1, 0,
                    0.5, -0.5, -0.5, 0, 0, 1,
                    0.5, -0.5, -0.5, 0, 0, 1,
                   -0.5, -0.5, -0.5, 1, 1, 0,
                   -0.5, -0.5, -0.5, 1, 1, 0,
                   -0.5,  0.5, -0.5, 1, 0, 0,

                   -0.5,  0.5,  0.5, 1, 0, 0,
                    0.5,  0.5,  0.5, 0, 1, 0,
                    0.5,  0.5,  0.5, 0, 1, 0,
                    0.5, -0.5,  0.5, 0, 0, 1,
                    0.5, -0.5,  0.5, 0, 0, 1,
                   -0.5, -0.5,  0.5, 1, 1, 0,
                   -0.5, -0.5,  0.5, 1, 1, 0,
                   -0.5,  0.5,  0.5, 1, 0, 0,

                   -0.5,  0.5, -0.5, 1, 0, 0,
                   -0.5,  0.5,  0.5, 1, 0, 0,
                    0.5,  0.5, -0.5, 0, 1, 0,
                    0.5,  0.5,  0.5, 0, 1, 0,
                    0.5, -0.5, -0.5, 0, 0, 1,
                    0.5, -0.5,  0.5, 0, 0, 1,
                   -0.5, -0.5, -0.5, 1, 1, 0,
                   -0.5, -0.5,  0.5, 1, 1, 0
};


void GfxWidget::render()
{
  render1();
}

void GfxWidget::render1()
{
  GFX::Context context(pixmap()->width(), pixmap()->height());

  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(context, program);

  context.setNear(1);

  GFX::mat4 trans1 = GFX::translationMatrix(0, 0, 10);
  GFX::mat4 trans2 = GFX::translationMatrix(0, 0, -10);
 
  GFX::mat4 rotateX = GFX::xRotationMatrix(angleY);
  GFX::mat4 rotateY = GFX::yRotationMatrix(angleX);

  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.5, 20);
  GFX::mat4 modelview = GFX::lookAtMatrix(0, 0, 5, 0, 0, 0, 0, 1, 0);

  vertexShader.transform = trans1 * rotateX * rotateY * trans2 * modelview * project;


  std::cout << "transform matrix:" << std::endl << vertexShader.transform << std::endl;

  context.zBuffer().clear(std::numeric_limits<double>::max());
  //context.disable(Context::GFX_ZBUFFER);

  renderer.drawLines(lines, 6 * 2 * 12, 6);

  double triangles[] = { -0.5, -0.5,  0.3, 1, 0, 0,
                          0.5, -0.5,  0.3, 0, 1, 0,
                          0.0,  0.5,  0.3, 0, 0, 1,
                         -0.5, -0.3, -0.5, 0, 0, 1,
                          0.5, -0.3, -0.5, 0, 1, 1,
                          0.0, -0.3,  0.5, 0, 1, 0 };

  renderer.drawTriangles(triangles, 36, 6);


  for (int i = 0; i < context.width(); ++i)
    for (int j = 0; j < context.height(); ++j) {
      const GFX::Color &color = context.colorBuffer()(i, j);
      m_image.setPixel(i, context.height() - j - 1, QColor(255 * color.r, 255 * color.g, 255 * color.b).rgb());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);

}

void GfxWidget::render2()
{
  GFX::Context context(pixmap()->width(), pixmap()->height());

  SimpleVertexShader vertexShader;
  SimpleFragmentShader fragmentShader;
  GFX::Program<SimpleVertexShader, SimpleFragmentShader> program(vertexShader, fragmentShader);
  GFX::Renderer<GFX::Program<SimpleVertexShader, SimpleFragmentShader> > renderer(context, program);

  context.setNear(1);

  GFX::mat4 scale = GFX::scaleMatrix(1.0);

  GFX::mat4 trans1 = GFX::translationMatrix(0, 0, 10);
  GFX::mat4 trans2 = GFX::translationMatrix(0, 0, 0);
 
  GFX::mat4 rotateX = GFX::xRotationMatrix(angleY);
  GFX::mat4 rotateY = GFX::yRotationMatrix(angleX);

  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.5, 20);
  GFX::mat4 modelview = GFX::lookAtMatrix(0, 0, 5, 0, 0, 0, 0, 1, 0);

  vertexShader.transform = trans1 * rotateX * rotateY * trans2 * modelview * project;


  std::cout << "transform matrix:" << std::endl << vertexShader.transform << std::endl;

  context.zBuffer().clear(std::numeric_limits<double>::max());
  //context.disable(Context::GFX_ZBUFFER);

  double triangles[] = {
     // front
    -1,  1, -1, 1, 0, 0, // d
     1, -1, -1, 0, 0, 1, // b
    -1, -1, -1, 1, 1, 0, // a
    -1,  1, -1, 1, 0, 0, // d
     1,  1, -1, 0, 1, 0, // c
     1, -1, -1, 0, 0, 1, // b
     // right
     1,  1, -1, 0, 1, 0, // c
     1, -1,  1, 1, 0, 0, // f
     1, -1, -1, 0, 0, 1, // b
     1,  1, -1, 0, 1, 0, // c
     1,  1,  1, 1, 1, 0, // g
     1, -1,  1, 1, 0, 0, // f
     // back
     1,  1,  1, 1, 1, 0, // g
    -1, -1,  1, 0, 1, 0, // e
     1, -1,  1, 1, 0, 0, // f
     1,  1,  1, 1, 1, 0, // g
    -1,  1,  1, 0, 0, 1, // h
    -1, -1,  1, 0, 1, 0, // e
     // left
    -1,  1,  1, 0, 0, 1, // h
    -1, -1, -1, 1, 1, 0, // a
    -1, -1,  1, 0, 1, 0, // e
    -1,  1,  1, 0, 0, 1, // h
    -1,  1, -1, 1, 0, 0, // d
    -1, -1, -1, 1, 1, 0, // a
     // top
    -1,  1,  1, 0, 0, 1, // h
     1,  1, -1, 0, 1, 0, // c
    -1,  1, -1, 1, 0, 0, // d
    -1,  1,  1, 0, 0, 1, // h
     1,  1,  1, 1, 1, 0, // g
     1,  1, -1, 0, 1, 0, // c
     // bottom
    -1, -1, -1, 1, 1, 0, // a
     1, -1,  1, 1, 0, 0, // f
    -1, -1,  1, 0, 1, 0, // e
    -1, -1, -1, 1, 1, 0, // a
     1, -1, -1, 0, 0, 1, // b
     1, -1,  1, 1, 0, 0, // f
  };


  renderer.drawTriangles(triangles, 36, 6);


  for (int i = 0; i < context.width(); ++i)
    for (int j = 0; j < context.height(); ++j) {
      const GFX::Color &color = context.colorBuffer()(i, j);
      m_image.setPixel(i, context.height() - j - 1, QColor(255 * color.r, 255 * color.g, 255 * color.b).rgb());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);

}
