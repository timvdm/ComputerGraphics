#include "../utils.h"
#include "../plugin.h"

#include "../libgfx/transform.h"
#include "../libgfx/mesh.h"
#include "../libgfx/utility.h"

#include "LSystem3D.h"
#include "render.h"


namespace CG {

  class ZBuffering : public Plugin
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

        std::vector<std::shared_ptr<GFX::Mesh> > meshes;
        std::vector<GFX::mat4> modelMatrices;
        std::vector<GFX::Color> colors;

        for (int i = 0; i < nrFigures; ++i) {
          std::string figureName = make_string("Figure", i);

          try {
            std::string type = conf[figureName]["type"].as_string_or_die();
            GFX::Color color = extractColor(conf[figureName]["color"]);
            colors.push_back(color);

            GFX::mat4 model = modelMatrix(figureName, conf);
            modelMatrices.push_back(model);

            if (type == "Cube") {

              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::cube();
              mesh->triangulate();
              meshes.push_back(mesh);

            } else if (type == "Tetrahedron") {

              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::tetrahedron();
              mesh->triangulate();
              meshes.push_back(mesh);

            } else if (type == "Octahedron") {

              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::octahedron();
              mesh->triangulate();
              meshes.push_back(mesh);

            } else if (type == "Icosahedron") {

              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::icosahedron();
              mesh->triangulate();
              meshes.push_back(mesh);

            } else if (type == "Dodecahedron") {

              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::dodecahedron();
              mesh->triangulate();
              meshes.push_back(mesh);

            } else if (type == "Cone") {

              int n = conf[figureName]["n"];
              GFX::Real h = conf[figureName]["height"].as_double_or_die();
              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::cone(n, h);
              mesh->triangulate();
              meshes.push_back(mesh);

            } else if (type == "Cylinder") {

              int n = conf[figureName]["n"];
              GFX::Real h = conf[figureName]["height"].as_double_or_die();
              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::cylinder(n, h);
              mesh->triangulate();
              meshes.push_back(mesh);

            } else if (type == "Sphere") {

              int n = conf[figureName]["n"];
              //renderMesh(*GFX::Mesh::sphere(n), color, project * model, lines);
              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::sphere(n);
              mesh->triangulate();
              meshes.push_back(mesh);

            } else if (type == "Torus") {

              int n = conf[figureName]["n"];
              int m = conf[figureName]["m"];
              GFX::Real R = conf[figureName]["R"].as_double_or_die();
              GFX::Real r = conf[figureName]["r"].as_double_or_die();
              std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::torus(n, m, R, r);
              mesh->triangulate();
              meshes.push_back(mesh);

            }

          } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return img::EasyImage();
          }
        }

        return draw_zbuffered_meshes(meshes, project, modelMatrices, colors, size, img::Color(255 * bgColor.r, 255 * bgColor.g, 255 * bgColor.b));
      }

  };

  PLUGIN_FACTORY("ZBuffering", ZBuffering)

}
