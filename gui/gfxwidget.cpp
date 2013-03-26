#include "gfxwidget.h"

GfxWidget::GfxWidget(int width, int height, QWidget *parent) : QLabel(parent),
    m_context(width, height), m_image(width, height, QImage::Format_RGB32),
    m_dragging(false), m_eyeZ(-5.0)
{
  QPixmap pm(width, height);
  setPixmap(pm);
  m_angleX = m_angleY = 0.0;
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
  
  m_eyeZ -= 0.001 * event->delta();
  
  update();
}
    
void GfxWidget::paintEvent(QPaintEvent *painter)
{
  m_fps.startRender();
  
  render();
  
  m_fps.stopRender();
  
  qDebug() << "FPS: " << m_fps.fps();
}
