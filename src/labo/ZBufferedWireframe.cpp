#include "../utils.h"
#include "../plugin.h"

#include <libgfx/transform.h>
#include <libgfx/mesh.h>
#include <libgfx/utility.h>

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

        mesh_to_lines3d(mesh, color, T, lines);

        return true;
      }

      std::shared_ptr<GFX::Mesh> createFractal(std::vector<GFX::vec4> &points0, std::vector<GFX::Mesh::Face> &faces,
          int nrIterations, double fractalScale)
      {
        // compute center
        GFX::vec4 center(GFX::vec4::Zero());
        for (std::size_t i = 0; i < points0.size(); ++i)
          center += points0[i];
        // center figure
        for (std::size_t i = 0; i < points0.size(); ++i)
          points0[i] -= center;

        std::size_t h = points0.size();
        std::vector<GFX::vec4> lastPoints = points0, points;

        for (int i = 0; i < nrIterations; ++i) {
          points.clear();

          for (std::size_t k = 0; k < lastPoints.size(); ++k) {
            std::size_t j = (k % h);

            // scale initial points
            std::vector<GFX::vec4> addPoints;
            for (std::size_t l = 0; l < h; ++l)
              addPoints.push_back(points0[l] / std::pow(fractalScale, i + 1));

            // translate small figure to origin
            GFX::vec4 point_j = addPoints[j];
            for (std::size_t l = 0; l < addPoints.size(); ++l)
              addPoints[l] -= point_j;

            // translate small figure to point on figure from previous iteration
            for (std::size_t l = 0; l < addPoints.size(); ++l)
              points.push_back(addPoints[l] + lastPoints[k]);
          }

          lastPoints.swap(points);
        }

        std::size_t offset = points0.size();

        std::size_t f = faces.size();
        std::size_t numFaces = f * std::pow(h, nrIterations);
        while (faces.size() < numFaces) {

          for (std::size_t i = 0; i < f; ++i) {
            faces.resize(faces.size() + 1);
            for (std::size_t j = 0; j < faces[i].size(); ++j) {
              faces.back().push_back(offset + faces[i][j]);
            }
          }

          offset += points0.size();
        }

        std::shared_ptr<GFX::Mesh> mesh(new GFX::Mesh);
        for (std::size_t i = 0; i < lastPoints.size(); ++i)
          mesh->addVertex(lastPoints[i].x(), lastPoints[i].y(), lastPoints[i].z());
        for (std::size_t i = 0; i < faces.size(); ++i)
          mesh->addFace(faces[i]);

        return mesh;
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

              mesh_to_lines3d(*GFX::Mesh::cube(), color, project * model, lines);

            } else if (type == "Tetrahedron") {

              mesh_to_lines3d(*GFX::Mesh::tetrahedron(), color, project * model, lines);

            } else if (type == "Octahedron") {

              mesh_to_lines3d(*GFX::Mesh::octahedron(), color, project * model, lines);

            } else if (type == "Icosahedron") {

              mesh_to_lines3d(*GFX::Mesh::icosahedron(), color, project * model, lines);

            } else if (type == "Dodecahedron") {

              mesh_to_lines3d(*GFX::Mesh::dodecahedron(), color, project * model, lines);

            } else if (type == "BuckyBall") {

              mesh_to_lines3d(*GFX::Mesh::buckyball(), color, project * model, lines);

            } else if (type == "Cone") {

              int n = conf[figureName]["n"];
              GFX::Real h = conf[figureName]["height"].as_double_or_die();
              mesh_to_lines3d(*GFX::Mesh::cone(n, h), color, project * model, lines);

            } else if (type == "Cylinder") {

              int n = conf[figureName]["n"];
              GFX::Real h = conf[figureName]["height"].as_double_or_die();
              mesh_to_lines3d(*GFX::Mesh::cylinder(n, h), color, project * model, lines);

            } else if (type == "Sphere") {

              int n = conf[figureName]["n"];
              mesh_to_lines3d(*GFX::Mesh::sphere(n), color, project * model, lines);

            } else if (type == "Torus") {

              int n = conf[figureName]["n"];
              int m = conf[figureName]["m"];
              GFX::Real R = conf[figureName]["R"].as_double_or_die();
              GFX::Real r = conf[figureName]["r"].as_double_or_die();
              mesh_to_lines3d(*GFX::Mesh::torus(n, m, R, r), color, project * model, lines);

            } else if (type == "3DLSystem") {

              //
              // 3D L-Systems
              //

              std::string inputfile = conf[figureName]["inputfile"].as_string_or_die();
              mesh_to_lines3d(*LSystem3D::generateMesh(inputfile), color, project * model, lines);

            } else if (type.substr(0, 7) == "Fractal") {

              //
              // 3D Fractals
              //

              int nrIterations = conf[figureName]["nrIterations"];
              double fractalScale = conf[figureName]["fractalScale"];

              std::shared_ptr<GFX::Mesh> unit;

              if (type == "FractalTetrahedron")
                unit = GFX::Mesh::tetrahedron();
              else if (type == "FractalCube")
                unit = GFX::Mesh::cube();
              else if (type == "FractalIcosahedron")
                unit = GFX::Mesh::icosahedron();
              else if (type == "FractalOctahedron")
                unit = GFX::Mesh::octahedron();
              else if (type == "FractalDodecahedron")
                unit = GFX::Mesh::dodecahedron();
              else if (type == "FractalBuckyBall")
                unit = GFX::Mesh::buckyball();

              std::vector<GFX::vec4> points0 = unit->vertices();
              std::vector<GFX::Mesh::Face> faces = unit->faces();

              std::shared_ptr<GFX::Mesh> mesh = createFractal(points0, faces, nrIterations, fractalScale);

              mesh_to_lines3d(*mesh, color, project * model, lines);

            } else if (type == "MengerSponge") {

              //
              // MengerSponge
              //

              int nrIterations = conf[figureName]["nrIterations"];

              std::vector<std::pair<GFX::vec3, GFX::vec3> > cubes(1, std::make_pair(GFX::vec3(-1, -1, -1), GFX::vec3(1, 1, 1)));

              for (int i = 0; i < nrIterations; ++i) {
                std::vector<std::pair<GFX::vec3, GFX::vec3> > nextCubes;

                // the new cube size
                GFX::Real delta = (cubes.front().second.x() - cubes.front().first.x()) / 3.0;


                for (auto cube : cubes) {
                  // the reference point
                  GFX::vec3 p = cube.first;

                  // front, bottom
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 0 * delta, p.y() + 0 * delta, p.z() + 0 * delta), GFX::vec3(p.x() + 1 * delta, p.y() + 1 * delta, p.z() + 1 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 1 * delta, p.y() + 0 * delta, p.z() + 0 * delta), GFX::vec3(p.x() + 2 * delta, p.y() + 1 * delta, p.z() + 1 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 2 * delta, p.y() + 0 * delta, p.z() + 0 * delta), GFX::vec3(p.x() + 3 * delta, p.y() + 1 * delta, p.z() + 1 * delta)));
                  // front, middle
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 0 * delta, p.y() + 1 * delta, p.z() + 0 * delta), GFX::vec3(p.x() + 1 * delta, p.y() + 2 * delta, p.z() + 1 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 2 * delta, p.y() + 1 * delta, p.z() + 0 * delta), GFX::vec3(p.x() + 3 * delta, p.y() + 2 * delta, p.z() + 1 * delta)));
                  // front, top
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 0 * delta, p.y() + 2 * delta, p.z() + 0 * delta), GFX::vec3(p.x() + 1 * delta, p.y() + 3 * delta, p.z() + 1 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 1 * delta, p.y() + 2 * delta, p.z() + 0 * delta), GFX::vec3(p.x() + 2 * delta, p.y() + 3 * delta, p.z() + 1 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 2 * delta, p.y() + 2 * delta, p.z() + 0 * delta), GFX::vec3(p.x() + 3 * delta, p.y() + 3 * delta, p.z() + 1 * delta)));

                  // middle, bottom
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 0 * delta, p.y() + 0 * delta, p.z() + 1 * delta), GFX::vec3(p.x() + 1 * delta, p.y() + 1 * delta, p.z() + 2 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 2 * delta, p.y() + 0 * delta, p.z() + 1 * delta), GFX::vec3(p.x() + 3 * delta, p.y() + 1 * delta, p.z() + 2 * delta)));
                  // middle, top
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 0 * delta, p.y() + 2 * delta, p.z() + 1 * delta), GFX::vec3(p.x() + 1 * delta, p.y() + 3 * delta, p.z() + 2 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 2 * delta, p.y() + 2 * delta, p.z() + 1 * delta), GFX::vec3(p.x() + 3 * delta, p.y() + 3 * delta, p.z() + 2 * delta)));

                  // back, bottom
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 0 * delta, p.y() + 0 * delta, p.z() + 2 * delta), GFX::vec3(p.x() + 1 * delta, p.y() + 1 * delta, p.z() + 3 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 1 * delta, p.y() + 0 * delta, p.z() + 2 * delta), GFX::vec3(p.x() + 2 * delta, p.y() + 1 * delta, p.z() + 3 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 2 * delta, p.y() + 0 * delta, p.z() + 2 * delta), GFX::vec3(p.x() + 3 * delta, p.y() + 1 * delta, p.z() + 3 * delta)));
                  // back, middle
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 0 * delta, p.y() + 1 * delta, p.z() + 2 * delta), GFX::vec3(p.x() + 1 * delta, p.y() + 2 * delta, p.z() + 3 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 2 * delta, p.y() + 1 * delta, p.z() + 2 * delta), GFX::vec3(p.x() + 3 * delta, p.y() + 2 * delta, p.z() + 3 * delta)));
                  // back, top
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 0 * delta, p.y() + 2 * delta, p.z() + 2 * delta), GFX::vec3(p.x() + 1 * delta, p.y() + 3 * delta, p.z() + 3 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 1 * delta, p.y() + 2 * delta, p.z() + 2 * delta), GFX::vec3(p.x() + 2 * delta, p.y() + 3 * delta, p.z() + 3 * delta)));
                  nextCubes.push_back(std::make_pair(GFX::vec3(p.x() + 2 * delta, p.y() + 2 * delta, p.z() + 2 * delta), GFX::vec3(p.x() + 3 * delta, p.y() + 3 * delta, p.z() + 3 * delta)));

                }

                cubes.swap(nextCubes);
              }

              for (auto cube : cubes) {
                std::shared_ptr<GFX::Mesh> mesh = GFX::Mesh::cube();

                // scale the cube mesh
                GFX::Real s = (cube.second.x() - cube.first.x()) / 2.0;
                for (GFX::vec4 &v : mesh->vertices()) {
                  v.x() *= s;
                  v.y() *= s;
                  v.z() *= s;
                }

                // translate the cube mesh
                GFX::vec3 c = (cube.first + cube.second) / 2.0;
                for (GFX::vec4 &v : mesh->vertices()) {
                  v.x() -= c.x();
                  v.y() -= c.y();
                  v.z() -= c.z();
                }

                mesh_to_lines3d(*mesh, color, project * model, lines);
              }

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
