#include "../utils.h"
#include "../plugin.h"

#include "../libgfx/transform.h"
#include "../libgfx/mesh.h"
#include "../libgfx/utility.h"

#include "LSystem3D.h"
#include "render.h"


namespace CG {

  class ZBufferedWireframe : public Plugin
  {
    public:
      GFX::Color extractColor(const ini::Entry &entry) const
      {
        try {
          std::vector<double> rgb = entry;
          return GFX::Color(255 * rgb.at(0), 255 * rgb.at(1), 255 * rgb.at(2));
        } catch (const std::exception &e) {
          std::cerr << "Could not parse color (using default black)" << std::endl;
        }

        return GFX::Color(0, 0, 0);
      }

      GFX::mat4 modelMatrix(const std::string &figureName, const ini::Configuration &conf)
      {
        GFX::mat4 m;

        // translation
        std::vector<double> center = conf[figureName]["center"];
        m = GFX::translationMatrix(center[0], center[1], center[2]);
        // rotation around Z
        GFX::Real angleZ = conf[figureName]["rotateZ"].as_double_or_die();
        m *= GFX::rotationMatrix(GFX::deg2rad(angleZ), 0, 0, 1);
        // rotation around Y
        GFX::Real angleY = conf[figureName]["rotateY"].as_double_or_die();
        m *= GFX::rotationMatrix(GFX::deg2rad(angleY), 0, 1, 0);
        // rotation around X
        GFX::Real angleX = conf[figureName]["rotateX"].as_double_or_die();
        m *= GFX::rotationMatrix(GFX::deg2rad(angleX), 1, 0, 0);
        // scale
        GFX::Real scale = conf[figureName]["scale"].as_double_or_die();
        m *= GFX::scaleMatrix(scale);

        return m;
      }

      bool renderLineDrawing(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines3D &lines, const GFX::mat4 &T)
      {
        int nrPoints;
        int nrLines;
        GFX::Color color;

        try {
          color = extractColor(conf[figureName]["color"]);
          nrPoints = conf[figureName]["nrPoints"];
          nrLines = conf[figureName]["nrLines"];
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        GFX::Mesh mesh;

        try {
          for (int j = 0; j < nrPoints; ++j)
            mesh.addVertex(conf[figureName][make_string("point", j)]);
          for (int j = 0; j < nrLines; ++j)
            mesh.addFace(conf[figureName][make_string("line", j)]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        renderMesh(mesh, color, T, lines);

        return true;
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

        GFX::mat4 project = GFX::projectionMatrix(eye[0], eye[1], eye[2]);

        GFX::Lines3D lines;

        for (int i = 0; i < nrFigures; ++i) {
          std::string figureName = make_string("Figure", i);

          try {
            std::string type = conf[figureName]["type"].as_string_or_die();
            GFX::Color color = extractColor(conf[figureName]["color"]);

            GFX::mat4 model = modelMatrix(figureName, conf);

            if (type == "LineDrawing") {

              if (!renderLineDrawing(figureName, conf, lines, project * model))
                return img::EasyImage();

            } else if (type == "Cube") {

              renderMesh(*GFX::Mesh::cube(), color, project * model, lines);

            } else if (type == "Tetrahedron") {

              renderMesh(*GFX::Mesh::tetrahedron(), color, project * model, lines);

            } else if (type == "Octahedron") {

              renderMesh(*GFX::Mesh::octahedron(), color, project * model, lines);

            } else if (type == "Icosahedron") {

              renderMesh(*GFX::Mesh::icosahedron(), color, project * model, lines);

            } else if (type == "Dodecahedron") {

              renderMesh(*GFX::Mesh::dodecahedron(), color, project * model, lines);

            } else if (type == "Cone") {

              int n = conf[figureName]["n"];
              GFX::Real h = conf[figureName]["height"].as_double_or_die();
              renderMesh(*GFX::Mesh::cone(n, h), color, project * model, lines);

            } else if (type == "Cylinder") {

              int n = conf[figureName]["n"];
              GFX::Real h = conf[figureName]["height"].as_double_or_die();
              renderMesh(*GFX::Mesh::cylinder(n, h), color, project * model, lines);

            } else if (type == "Sphere") {

              int n = conf[figureName]["n"];
              renderMesh(*GFX::Mesh::sphere(n), color, project * model, lines);

            } else if (type == "Torus") {

              int n = conf[figureName]["n"];
              int m = conf[figureName]["m"];
              GFX::Real R = conf[figureName]["R"].as_double_or_die();
              GFX::Real r = conf[figureName]["r"].as_double_or_die();
              renderMesh(*GFX::Mesh::torus(n, m, R, r), color, project * model, lines);

            } else if (type == "3DLSystem") {

              std::string inputfile = conf[figureName]["inputfile"].as_string_or_die();
              renderMesh(*LSystem3D::generateMesh(inputfile), color, project * model, lines);

            }

          } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return img::EasyImage();
          }
        }

        return draw_zbuffered_lines(lines, size, img::Color(255 * bgColor.r, 255 * bgColor.g, 255 * bgColor.b));
      }

  };

  PLUGIN_FACTORY("ZBufferedWireframe", ZBufferedWireframe)

}
