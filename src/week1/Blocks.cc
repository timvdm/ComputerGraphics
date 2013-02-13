#include "../utils.h"
#include "../plugin.h"

namespace CG {

  class Blocks : public Plugin
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
        int width, height;
        int nrXBlocks, nrYBlocks;
        img::Color white, black;
        bool invertColors = false;

        try {
          width = conf["ImageProperties"]["width"];
          height = conf["ImageProperties"]["height"];
          nrXBlocks = conf["BlockProperties"]["nrXBlocks"];
          nrYBlocks = conf["BlockProperties"]["nrYBlocks"];
          white = extractColor(conf["BlockProperties"]["colorWhite"]);
          black = extractColor(conf["BlockProperties"]["colorBlack"]);
          conf["BlockProperties"]["invertColors"].as_bool_if_exists(invertColors);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return img::EasyImage();
        }

        img::EasyImage image(width, height);

        // block width/height
        int blockWidth = width / nrXBlocks;
        int blockHeight = height / nrYBlocks;

        for (int i = 0; i < width; i++) {
          for (int j = 0; j < height; j++) {
            int Bx = i / blockWidth;
            int By = j / blockHeight;
            const img::Color &color = ((By + Bx) % 2) ? black : white;
            image(i, j) = color;
          }
        }

        return image;
      }

  };

  PLUGIN_FACTORY("IntroBlocks", Blocks);

}
