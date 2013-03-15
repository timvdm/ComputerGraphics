#ifndef GFX_GFXWIDGET_H
#define GFX_GFXWIDGET_H

#include <QtGui>

#include <libgfx/context.h>
#include <libgfx/spherecoord.h>
#include <libgfx/fps.h>

class GfxWidget : public QLabel
{
  Q_OBJECT

  public:
    GfxWidget(QWidget *parent = 0);

    void render();
    
    void render1();
    void render2();

  protected:
    virtual void paintEvent(QPaintEvent *painter);

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);


    QImage m_image;
    GFX::Context m_context;
    GFX::SphereCoord m_eye;
    GFX::Point2D m_mousePos;
    GFX::FramesPerSecond m_fps;
    bool m_dragging;
    double angleX;
    double angleY;
    double m_eyeZ;
};

#endif
