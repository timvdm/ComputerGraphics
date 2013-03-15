#include <libgfx/render2d.h>

#include "../src/utils/EasyImage.h"

#include <fstream>

using namespace GFX;

struct EasyImageCanvas
{
  EasyImageCanvas(int width, int height, const Color &color) : image(width, height, img::Color(color.r, color.g, color.b))
  {
  }

  void setPixel(int x, int y, const Color &color)
  {
    image(x, y) = img::Color(color.r, color.g, color.b);
  }

  img::EasyImage image;
};

void test_Render2D_drawLine()
{
  EasyImageCanvas canvas(500, 500, Color::black());
  Render2D<EasyImageCanvas> painter(canvas);

  painter.drawLine(100, 100, 400, 100, Color::red());
  painter.drawLine(400, 200, 100, 200, Color::red());
  
  painter.drawLine(100, 100, 100, 400, Color::green());
  painter.drawLine(200, 400, 200, 100, Color::green());

  painter.drawLine(100, 100, 400, 200, Color::yellow());
  painter.drawLine(400, 100, 100, 200, Color::blue());
  
  painter.drawLine(100, 100, 200, 400, Color::yellow());
  painter.drawLine(100, 400, 200, 100, Color::blue());

  std::ofstream ofs("test_Render2D_drawLine.bmp", std::ios_base::out | std::ios_base::binary);
  ofs << canvas.image;
}

void test_Render2D_drawCircle()
{
  EasyImageCanvas canvas(500, 500, Color::black());
  Render2D<EasyImageCanvas> painter(canvas);

  painter.drawCircle(250, 250, 50, Color::yellow());
  painter.drawCircle(250, 250, 100, Color::red());
  painter.drawCircle(250, 250, 150, Color::green());
  painter.drawCircle(250, 250, 200, Color::blue());

  std::ofstream ofs("test_Render2D_drawCircle.bmp", std::ios_base::out | std::ios_base::binary);
  ofs << canvas.image;
}

void test_Render2D_drawEllipse()
{
  EasyImageCanvas canvas(500, 500, Color::black());
  Render2D<EasyImageCanvas> painter(canvas);

  painter.drawEllipse(250, 250, 50, 200, Color::yellow());
  painter.drawEllipse(250, 250, 230, 100, Color::red());

  std::ofstream ofs("test_Render2D_drawEllipse.bmp", std::ios_base::out | std::ios_base::binary);
  ofs << canvas.image;
}

void test_Render2D_drawBezierCurve()
{
  EasyImageCanvas canvas(500, 500, Color::black());
  Render2D<EasyImageCanvas> painter(canvas);

  painter.drawBezierCurve(50, 50, 30, 200, 450, 450, 350, 30, Color::green());
  painter.drawBezierCurve(150, 450, 0, 0, 500, 0, 350, 450, Color::red());
  
  painter.drawBezierCurve(0, 0, 0, 250, 250, 250, 250, 250, Color::blue());
  painter.drawBezierCurve(250, 250, 250, 250, 499, 250, 499, 499, Color::blue());

  std::ofstream ofs("test_Render2D_drawBezierCurve.bmp", std::ios_base::out | std::ios_base::binary);
  ofs << canvas.image;
}

int main()
{
  test_Render2D_drawLine();
  test_Render2D_drawCircle();
  test_Render2D_drawEllipse();
  test_Render2D_drawBezierCurve();
}
