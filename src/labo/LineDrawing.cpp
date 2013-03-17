#include "../utils.h"
#include "../plugin.h"

#include "render.h"

#include <algorithm>

namespace CG {

  class LineDrawing : public Plugin
  {
    public:
      img::Color extractColor(const ini::Entry &entry) const
      {
        try {
          std::vector<double> rgb = entry;
          return img::Color(rgb.at(0) * 255, rgb.at(1) * 255, rgb.at(2) * 255);
        } catch (const std::exception &e) {
          std::cerr << "Could not parse color (using default black)" << std::endl;
        }
        
        return img::Color(0, 0, 0);
      }

      img::EasyImage image(const ini::Configuration &conf)
      {
        std::vector<double> vPoints;
        std::vector<int> vLineIndexes;
        img::Color bgColor;
        std::vector<double> lineColor;
        int size;

        try {
          vPoints = conf["LineDrawingProperties"]["points"];
          vLineIndexes = conf["LineDrawingProperties"]["lineIndexes"];
          bgColor = extractColor(conf["LineDrawingProperties"]["bgColor"]);
          lineColor = conf["LineDrawingProperties"]["lineColor"];
          size = conf["LineDrawingProperties"]["size"];
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return img::EasyImage();
        }

        // make sure both vectors are a multiple of 2
        assert((vPoints.size() % 2) == 0);
        assert((vLineIndexes.size() % 2) == 0);

        // extract points
        std::vector<GFX::Point2D> points;
        for (std::size_t i = 0; i < vPoints.size(); i += 2)
          points.push_back(GFX::Point2D(vPoints[i], vPoints[i + 1]));

        // make sure the line point indexes are valid
        assert(*std::min_element(vLineIndexes.begin(), vLineIndexes.end()) >= 0);
        assert(*std::max_element(vLineIndexes.begin(), vLineIndexes.end()) < points.size());

        // extract lines
        GFX::Lines2D lines;
        for (std::size_t i = 0; i < vLineIndexes.size(); i += 2)
          lines.push_back(GFX::Line2D(points[vLineIndexes[i]], points[vLineIndexes[i + 1]], lineColor));

        // points copied to lines, not needed anymore
        points.clear();

        return draw_lines(lines, size, bgColor);
      }

  };

  PLUGIN_FACTORY("LineDrawing", LineDrawing);

}
