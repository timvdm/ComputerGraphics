#include "../utils.h"
#include "../plugin.h"

namespace CG {

  class Lines : public Plugin
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

      void quarterCircle(img::EasyImage &image, int Hi, int Wi, int N, const img::Color &color, int x, int y)
      {
        int Hs = Hi / (N - 1);
        int Ws = Wi / (N / 1);

        for (int i = 0; i < N; ++i)
          image.draw_line(i * Ws, y, x, i * Hs, color);
      }

      img::EasyImage image(const ini::Configuration &conf)
      {
        int width, height;
        std::string figure;
        int nrLines;
        img::Color backgroundColor, lineColor;

        try {
          width = conf["ImageProperties"]["width"];
          height = conf["ImageProperties"]["height"];
          figure = conf["LineProperties"]["figure"].as_string_or_die();
          nrLines = conf["LineProperties"]["nrLines"];
          backgroundColor = extractColor(conf["LineProperties"]["backgroundcolor"]);
          lineColor = extractColor(conf["LineProperties"]["lineColor"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return img::EasyImage();
        }

        img::EasyImage image(width, height, backgroundColor);

        if (figure == "QuarterCircle") {
          quarterCircle(image, height, width, nrLines, lineColor, 0, height - 1);
        } else if (figure == "Eye") {
          quarterCircle(image, height, width, nrLines, lineColor, 0, height - 1);
          quarterCircle(image, height, width, nrLines, lineColor, width - 1, 0);
        } else if (figure == "Diamond") {
          quarterCircle(image, height / 2, width / 2, nrLines, lineColor, width / 2, height / 2);
        }

        return image;
      }

  };

  PLUGIN_FACTORY("IntroLines", Lines);

}
