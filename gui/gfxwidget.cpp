#include "gfxwidget.h"

GfxWidget::GfxWidget(int width, int height, QWidget *parent) : QLabel(parent),
    m_context(width, height), m_image(width, height, QImage::Format_RGB32),
    m_dragging(false), m_eyeZ(1.0)
{
  QPixmap pm(width, height);
  setPixmap(pm);
  m_angleX = m_angleY = 0.0;
}
    
void GfxWidget::copyColorBufferToImage()
{
  for (int i = 0; i < m_context.width(); ++i)
    for (int j = 0; j < m_context.height(); ++j) {
      const GFX::Color &color = m_context.colorBuffer()(i, j);
      //image().setPixel(i, m_context.height() - j - 1, color.toARGB());
      image().setPixel(i, j, color.toARGB());
    }
  
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);
}

void GfxWidget::mouseMoveEvent(QMouseEvent *event)
{
  //qDebug() << "mouseMoveEvent";

  if (!m_dragging)
    return;
  
  int dx = m_mousePos.x - event->x();
  int dy = m_mousePos.y - event->y();
  
  m_angleX += dx * 5e-3;
  m_angleY += dy * 5e-3;

  update();

  m_mousePos = GFX::Point2D(event->x(), event->y());
}

void GfxWidget::mousePressEvent(QMouseEvent *event)
{
  //qDebug() << "mousePressEvent";

  m_dragging = true;
  m_mousePos = GFX::Point2D(event->x(), event->y());
}

void GfxWidget::mouseReleaseEvent(QMouseEvent *event)
{
  //qDebug() << "mouseReleaseEvent";
  
  m_dragging = false;
  
  int dx = m_mousePos.x - event->x();
  int dy = m_mousePos.y - event->y();
  
  m_angleX += dx * 5e-3;
  m_angleY += dy * 5e-3;

  update();
}
    
void GfxWidget::wheelEvent(QWheelEvent *event)
{
  //qDebug() << "delta = " << event->delta();
  
  m_eyeZ += 0.001 * event->delta();
  
  update();
}
    
void GfxWidget::paintEvent(QPaintEvent *painter)
{
  m_fps.startRender();
  
  render();
  
  m_fps.stopRender();
  
  qDebug() << "FPS: " << m_fps.fps();
}

void GfxWidget::updatePixmap(int index, const GFX::Buffer<GFX::Real> &zBuffer)
{
  // find depth range
  GFX::Real min = std::numeric_limits<GFX::Real>::max();
  GFX::Real max = std::numeric_limits<GFX::Real>::min();
  for (int i = 0; i < zBuffer.width(); ++i)
    for (int j = 0; j < zBuffer.height(); ++j) {
      if (zBuffer(i, j) == std::numeric_limits<GFX::Real>::max())
        continue;
      if (zBuffer(i, j) > max)
        max = zBuffer(i, j);
      if (zBuffer(i, j) < min)
        min = zBuffer(i, j);
    }

  QImage image(zBuffer.width(), zBuffer.height(), QImage::Format_RGB32);
  for (int i = 0; i < zBuffer.width(); ++i)
    for (int j = 0; j < zBuffer.height(); ++j) {
      GFX::Real z = (zBuffer(i, j) - min) / static_cast<GFX::Real>(max - min);
      const GFX::Color color(255 * z, 255 * z, 255 * z);
      image.setPixel(i, j, color.toARGB());
    }

  updatePixmap(index, QPixmap::fromImage(image));
}

void GfxWidget::updatePixmap(int index, const GFX::Buffer<GFX::Color> &buffer)
{
  QImage image(buffer.width(), buffer.height(), QImage::Format_RGB32);
  for (int i = 0; i < buffer.width(); ++i)
    for (int j = 0; j < buffer.height(); ++j) {
      const GFX::Color &color = buffer(i, j);
      image.setPixel(i, j, color.toARGB());
    }

  updatePixmap(index, QPixmap::fromImage(image));
}

void GfxWidget::updatePixmap(int index, const QPixmap &pixmap)
{
  while (index >= m_pixmaps.size())
    m_pixmaps.push_back(new QLabel);
  m_pixmaps[index]->setPixmap(pixmap);
  m_pixmaps[index]->show();
}

