#ifndef GFX_GFXWIDGET_H
#define GFX_GFXWIDGET_H

#include <QtGui>

#include <libgfx/point2d.h>
#include <libgfx/context.h>
#include <libgfx/fps.h>

class GfxWidget : public QLabel
{
  Q_OBJECT

  public:
    GfxWidget(int width, int height, QWidget *parent = 0);

    virtual void render() = 0;

    double angleX() const
    {
      return m_angleX;
    }

    double angleY() const
    {
      return m_angleY;
    }

    double eyeZ() const
    {
      return m_eyeZ;
    }

    void setEyeZ(double eyeZ)
    {
      m_eyeZ = eyeZ;
    }

    const GFX::Context& context() const
    {
      return m_context;
    }

    GFX::Context& context()
    {
      return m_context;
    }

    const QImage& image() const
    {
      return m_image;
    }

    QImage& image()
    {
      return m_image;
    }
    
    void copyColorBufferToImage();

    void updatePixmap(int index, const GFX::Buffer<GFX::Real> &zBuffer);

    void updatePixmap(int index, const GFX::Buffer<GFX::Color> &buffer);
    void updatePixmap(int index, const GFX::Buffer<GFX::ColorF> &buffer);

    void updatePixmap(int index, const QPixmap &pixmap);
    
  protected:
    virtual void paintEvent(QPaintEvent *painter);

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    virtual void resizeEvent(QResizeEvent *event);

  private:
    GFX::Context m_context;
    QImage m_image;

    double m_angleX;
    double m_angleY;
    double m_eyeZ;

    GFX::Point2D m_mousePos;
    GFX::FramesPerSecond m_fps;
    bool m_dragging;

    std::vector<QLabel*> m_pixmaps;
};

#endif
