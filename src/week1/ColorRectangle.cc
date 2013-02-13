#include "../utils.h"
#include "../plugin.h"

namespace CG {

  class ColorRectangle : public Plugin
  {
    public:
      img::EasyImage image(const ini::Configuration &conf)
      {
        int width;
        int height;

        try {
          width = conf["ImageProperties"]["width"].as_int_or_die();
          height = conf["ImageProperties"]["height"].as_int_or_die();
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return img::EasyImage();
        }

        img::EasyImage image(width, height);

        for (int i = 0; i < width; i++) {
          for (int j = 0; j < height; j++) {
            image(i, j).red = i;
            image(i, j).green = j;
            image(i, j).blue = (i + j) % 256;
          }
        }

        return image;
      }

  };

  PLUGIN_FACTORY("IntroColorRectangle", ColorRectangle);

}
