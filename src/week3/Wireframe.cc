#include "../utils.h"
#include "../plugin.h"

#include "../libgfx/lines2d.h"
#include "../libgfx/transform.h"
#include "../libgfx/mesh.h"


namespace CG {

  class Wireframe : public Plugin
  {
    public:
      GFX::Color extractColor(const ini::Entry &entry) const
      {
        try {
          std::vector<double> rgb = entry;
          return GFX::Color(rgb.at(0), rgb.at(1), rgb.at(2));
        } catch (const std::exception &e) {
          std::cerr << "Could not parse color (using default black)" << std::endl;
        }
        
        return GFX::Color(0, 0, 0);
      }

      img::EasyImage image(const ini::Configuration &conf)
      {
        int size;
        std::vector<double> eye;
        GFX::Color bgColor;
        int nrFigures;

        // read General section from *.ini file
        try {
          size = conf["General"]["size"];
          eye = conf["General"]["eye"];
          bgColor = extractColor(conf["General"]["backgroundcolor"]);
          nrFigures = conf["General"]["nrFigures"];
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return img::EasyImage();
        }
        
        GFX::mat4 T = GFX::projectionMatrix(eye[0], eye[1], eye[2]);

        std::cout << "T: " << std::endl << T << std::endl;

        std::vector<GFX::Mesh> meshes;
        GFX::Lines2D lines;

        for (int i = 0; i < nrFigures; ++i) {
          std::string figureName = make_string("Figure", i);

          double scale;
          double rotateX;
          double rotateY;
          double rotateZ;
          Vector3D center;
          int nrPoints;
          int nrLines;
          GFX::Color color;

          try {
            std::string type = conf[figureName]["type"].as_string_or_die();
            assert(type == "LineDrawing");
            scale = conf[figureName]["scale"];
            rotateX = conf[figureName]["rotateX"];
            rotateY = conf[figureName]["rotateY"];
            rotateZ = conf[figureName]["rotateZ"];
            std::vector<double> centerDoubles = conf[figureName]["center"];
            center.x = centerDoubles[0];
            center.y = centerDoubles[1];
            center.z = centerDoubles[2];
            color = extractColor(conf[figureName]["color"]);
            nrPoints = conf[figureName]["nrPoints"];
            nrLines = conf[figureName]["nrLines"];
          } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return img::EasyImage();
          }

          meshes.resize(meshes.size() + 1);

          try {
            for (int j = 0; j < nrPoints; ++j)
              meshes.back().addVertex(conf[figureName][make_string("point", j)]);
            for (int j = 0; j < nrLines; ++j)
              meshes.back().addFace(conf[figureName][make_string("line", j)]);
          } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return img::EasyImage();
          }


          for (std::size_t i = 0; i < meshes.size(); ++i) {
            for (std::size_t j = 0; j < meshes[i].faces().size(); ++j) {
              GFX::vec4 p1 = meshes[i].vertices()[meshes[i].faces()[j][0]];
              GFX::vec4 p2 = meshes[i].vertices()[meshes[i].faces()[j][1]];

              std::cout << GFX::print(p1) << " - " << GFX::print(p2) << " --> ";

              p1 = T * p1;
              p2 = T * p2;
              
              std::cout << GFX::print(p1) << " - " << GFX::print(p2) << " --> ";

              GFX::Point2D projP1(p1.x() / -p1.z(), p1.y() / -p1.z());
              GFX::Point2D projP2(p2.x() / -p2.z(), p2.y() / -p2.z());

              std::cout << projP1 << " - " << projP2 << std::endl;



              lines.push_back(GFX::Line2D(projP1, projP2, color));
            }
          }

        }




        return draw_lines(lines, size, img::Color(255 * bgColor.r, 255 * bgColor.g, 255 * bgColor.b));
      }

  };

  PLUGIN_FACTORY("Wireframe", Wireframe);

}
