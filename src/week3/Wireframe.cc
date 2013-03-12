#include "../utils.h"
#include "../plugin.h"

#include "../lines2d.h"

#include "../libxyz/mesh.h"
#include "../libxyz/polarcoord.h"


namespace CG {

  Matrix projectionMatrix(const XYZ::PolarCoord &eye)
  {
    Matrix T;
    T(1, 1) = - std::sin(eye.theta);
    T(1, 2) = - std::cos(eye.theta) * std::cos(eye.phi);
    T(1, 3) =   std::cos(eye.theta) * std::sin(eye.phi);
    //T(1, 4) =   0.0;
    T(2, 1) =   std::cos(eye.theta);
    T(2, 2) = - std::sin(eye.theta) * std::cos(eye.phi);
    T(2, 3) =   std::sin(eye.theta) * std::sin(eye.phi);
    //T(2, 4) =   0.0;
    //T(3, 1) =   0.0;
    T(3, 2) =   std::sin(eye.phi);
    T(3, 3) =   std::cos(eye.phi);
    //T(3, 4) =   0.0;
    //T(4, 1) =   0.0;
    //T(4, 2) =   0.0;
    T(4, 3) = - eye.r;
    T(4, 4) =   1.0;
    return T;
  }

  inline int nearest(double value)
  {
    return (value < 0.0) ? value - 0.5 : value + 0.5;
  }

  typedef Vector3D Vertex;
  typedef std::vector<Vertex> VertexArray;

  typedef std::vector<Color> ColorArray;

  class Wireframe : public Plugin
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
        int size;
        std::vector<double> eye;
        img::Color bgColor;
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

        XYZ::PolarCoord peye(eye);

        Matrix T = projectionMatrix(XYZ::PolarCoord(eye));

        std::vector<XYZ::Mesh> meshes;
        Lines2D lines;

        for (int i = 0; i < nrFigures; ++i) {
          std::string figureName = make_string("Figure", i);

          double scale;
          double rotateX;
          double rotateY;
          double rotateZ;
          Vector3D center;
          int nrPoints;
          int nrLines;
          img::Color color;

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
              Vector3D p1 = meshes[i].vertices()[meshes[i].faces()[j][0]];
              Vector3D p2 = meshes[i].vertices()[meshes[i].faces()[j][1]];
              p1 = p1 * T;
              p2 = p2 * T;

              Point2D projP1(p1.x / -p1.z, p1.y / -p1.z);
              Point2D projP2(p2.x / -p2.z, p2.y / -p2.z);
              lines.push_back(Line2D(projP1, projP2, color));
            }
          }

        }




        return draw_lines(lines, size, bgColor);
      }

  };

  PLUGIN_FACTORY("Wireframe", Wireframe);

}
