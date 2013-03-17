#include "../utils.h"
#include "../plugin.h"

#include "../libgfx/lines2d.h"
#include "../libgfx/transform.h"
#include "../libgfx/mesh.h"
#include "../libgfx/utility.h"

#include "LSystem3D.h"


namespace CG {

  class Wireframe : public Plugin
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
        double angleZ = conf[figureName]["rotateZ"];
        m *= GFX::rotationMatrix(GFX::deg2rad(angleZ), 0, 0, 1);
        // rotation around Y
        double angleY = conf[figureName]["rotateY"];
        m *= GFX::rotationMatrix(GFX::deg2rad(angleY), 0, 1, 0);
        // rotation around X
        double angleX = conf[figureName]["rotateX"];
        m *= GFX::rotationMatrix(GFX::deg2rad(angleX), 1, 0, 0);
        // scale
        double scale = conf[figureName]["scale"];
        m *= GFX::scaleMatrix(scale);

        return m;
      }

      void renderMesh(const GFX::Mesh &mesh, const GFX::Color &color, const GFX::mat4 &T, GFX::Lines2D &lines)
      {
        GFX::vec4 p1, p2;
        for (std::size_t i = 0; i < mesh.faces().size(); ++i) {
          const std::vector<int> &face = mesh.faces()[i];
          for (std::size_t j = 0; j < face.size(); ++j) {
            if (!j) {
              p1 = mesh.vertices()[face[face.size() - 1]];
              p2 = mesh.vertices()[face[0]];
            } else {
              p1 = mesh.vertices()[face[j-1]];
              p2 = mesh.vertices()[face[j]];
            }

            p1 = T * p1;
            p2 = T * p2;

            GFX::Point2D projP1(p1.x() / -p1.z(), p1.y() / -p1.z());
            GFX::Point2D projP2(p2.x() / -p2.z(), p2.y() / -p2.z());

            lines.push_back(GFX::Line2D(projP1, projP2, color));
          }
        }
      }
      
      bool renderLineDrawing(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
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

      bool renderCube(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        GFX::Color color;

        try {
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        GFX::Mesh mesh;

        mesh.addVertex( 1, -1, -1);
        mesh.addVertex(-1,  1, -1);
        mesh.addVertex( 1,  1,  1);
        mesh.addVertex(-1, -1,  1);
        mesh.addVertex( 1,  1, -1);
        mesh.addVertex(-1, -1, -1);
        mesh.addVertex( 1, -1,  1);
        mesh.addVertex(-1,  1,  1);

        mesh.addFace(0, 4, 2, 6);
        mesh.addFace(4, 1, 7, 2);
        mesh.addFace(1, 5, 3, 7);
        mesh.addFace(5, 0, 6, 3);
        mesh.addFace(6, 2, 7, 3);
        mesh.addFace(0, 5, 1, 4);

        renderMesh(mesh, color, T, lines);

        return true;
      }

      bool renderTetrahedron(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        GFX::Color color;

        try {
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        GFX::Mesh mesh;

        mesh.addVertex( 1, -1, -1);
        mesh.addVertex(-1,  1, -1);
        mesh.addVertex( 1,  1,  1);
        mesh.addVertex(-1, -1,  1);

        mesh.addFace(0, 1, 2);
        mesh.addFace(1, 3, 2);
        mesh.addFace(0, 1, 3);
        mesh.addFace(0, 2, 3);

        renderMesh(mesh, color, T, lines);

        return true;
      }

      bool renderOctahedron(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        GFX::Color color;

        try {
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        GFX::Mesh mesh;

        mesh.addVertex( 1,  0,  0);
        mesh.addVertex( 0,  1,  0);
        mesh.addVertex(-1,  0,  0);
        mesh.addVertex( 0, -1,  0);
        mesh.addVertex( 0,  0, -1);
        mesh.addVertex( 0,  0,  1);

        mesh.addFace(0, 1, 5);
        mesh.addFace(1, 2, 5);
        mesh.addFace(2, 3, 5);
        mesh.addFace(3, 0, 5);
        mesh.addFace(2, 0, 4);
        mesh.addFace(2, 1, 4);
        mesh.addFace(3, 2, 4);
        mesh.addFace(0, 3, 4);

        renderMesh(mesh, color, T, lines);

        return true;
      }

      bool renderIcosahedron(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        GFX::Color color;

        try {
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        GFX::Mesh mesh;

        double sqrt_5_2 = std::sqrt(5.0) / 2.0;

        mesh.addVertex(0.0, 0.0, sqrt_5_2);

        for (int i = 0; i < 5; ++i)
          mesh.addVertex(std::cos(i * 2.0 * M_PI / 5.0), std::sin(i * 2.0 * M_PI / 5.0), 0.5);
        
        for (int i = 0; i < 5; ++i)
          mesh.addVertex(std::cos(M_PI / 5.0 + i * 2.0 * M_PI / 5.0), std::sin(M_PI / 5.0 + i * 2.0 * M_PI / 5.0), -0.5);

        mesh.addVertex(0.0, 0.0, -sqrt_5_2);

        mesh.addFace(0, 1, 2);
        mesh.addFace(0, 2, 3);
        mesh.addFace(0, 3, 4);
        mesh.addFace(0, 4, 5);

        mesh.addFace(0, 5, 1);
        mesh.addFace(1, 6, 2);
        mesh.addFace(2, 6, 7);
        mesh.addFace(2, 7, 3);
        
        mesh.addFace(3, 7, 8);
        mesh.addFace(3, 8, 4);
        mesh.addFace(4, 8, 9);
        mesh.addFace(4, 9, 5);
        
        mesh.addFace(5, 9, 10);
        mesh.addFace(5, 10, 1);
        mesh.addFace(1, 10, 6);
        mesh.addFace(11, 7, 6);
        
        mesh.addFace(11, 8, 7);
        mesh.addFace(11, 9, 8);
        mesh.addFace(11, 10, 9);
        mesh.addFace(11, 6, 10);
        
        renderMesh(mesh, color, T, lines);

        return true;
      }

      bool renderDodecahedron(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        GFX::Color color;

        try {
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        GFX::Mesh mesh;
      
        double sqrt_5_2 = std::sqrt(5.0) / 2.0;
        double pi_2_5 = 2.0 * M_PI / 5.0;
        double pi_5 = M_PI / 5.0;

        double xyz[] = {
          0.0, 0.0, sqrt_5_2,
          std::cos(0.0 * pi_2_5), std::sin(0.0 * pi_2_5), 0.5,
          std::cos(1.0 * pi_2_5), std::sin(1.0 * pi_2_5), 0.5,
          std::cos(2.0 * pi_2_5), std::sin(2.0 * pi_2_5), 0.5,
          std::cos(3.0 * pi_2_5), std::sin(3.0 * pi_2_5), 0.5,
          std::cos(4.0 * pi_2_5), std::sin(4.0 * pi_2_5), 0.5,
          std::cos(pi_5 + 0.0 * pi_2_5), std::sin(pi_5 + 0.0 * pi_2_5), -0.5,
          std::cos(pi_5 + 1.0 * pi_2_5), std::sin(pi_5 + 1.0 * pi_2_5), -0.5,
          std::cos(pi_5 + 2.0 * pi_2_5), std::sin(pi_5 + 2.0 * pi_2_5), -0.5,
          std::cos(pi_5 + 3.0 * pi_2_5), std::sin(pi_5 + 3.0 * pi_2_5), -0.5,
          std::cos(pi_5 + 4.0 * pi_2_5), std::sin(pi_5 + 4.0 * pi_2_5), -0.5,
          0.0, 0.0, -sqrt_5_2
        };
        
        GFX::vec3 *verts = reinterpret_cast<GFX::vec3*>(xyz);

        int faces[] = {
          0, 1, 2,
          0, 2, 3,
          0, 3, 4,
          0, 4, 5,
          0, 5, 1,
          1, 6, 2,
          2, 6, 7,
          2, 7, 3,
          3, 7, 8,
          3, 8, 4,
          4, 8, 9,
          4, 9, 5,
          5, 9, 10,
          5, 10, 1,
          1, 10, 6,
          11, 7, 6,
          11, 8, 7,
          11, 9, 8,
          11, 10, 9,
          11, 6, 10
        };

        for (int i = 0; i < 20; ++i) {
          int *face = faces + 3 * i;
          GFX::vec3 point((verts[face[0]] + verts[face[1]] + verts[face[2]]) / 3.0);
          mesh.addVertex(point);
        }

        mesh.addFace(0, 1, 2, 3, 4);
        mesh.addFace(0, 5, 6, 7, 1);
        mesh.addFace(1, 7, 8, 9, 2);
        mesh.addFace(2, 9, 10, 11, 3);
        
        mesh.addFace(3, 11, 12, 13, 4);
        mesh.addFace(4, 13, 14, 5, 0);
        mesh.addFace(19, 18, 17, 16, 15);
        mesh.addFace(19, 14, 13, 12, 18);
        
        mesh.addFace(18, 12, 11, 10, 17);
        mesh.addFace(17, 10, 9, 8, 16);
        mesh.addFace(16, 8, 7, 6, 15);
        mesh.addFace(15, 6, 5, 14, 19);
        
        renderMesh(mesh, color, T, lines);

        return true;
      }

      bool renderCone(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        int n;
        double h;
        GFX::Color color;

        try {
          n = conf[figureName]["n"];
          h = conf[figureName]["height"];
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        GFX::Mesh mesh;

        for (int i = 0; i < n; ++i)
          mesh.addVertex(std::cos(2.0 * i * M_PI / n), std::sin(2.0 * i * M_PI / n), 0.0);

        mesh.addVertex(0.0, 0.0, h);

        for (int i = 0; i < n; ++i)
          mesh.addFace(i, (i + 1) % n, n);

        std::vector<int> face;
        for (int i = 0; i < n; ++i)
          face.push_back(n - i - 1);
        mesh.addFace(face);

        renderMesh(mesh, color, T, lines);

        return true;
      }

      bool renderCylinder(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        int n;
        double h;
        GFX::Color color;

        try {
          n = conf[figureName]["n"];
          h = conf[figureName]["height"];
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }

        GFX::Mesh mesh;

        for (int i = 0; i < n; ++i) {
          double x = std::cos(2.0 * i * M_PI / n);
          double y = std::sin(2.0 * i * M_PI / n);
          mesh.addVertex(x, y, 0.0);
        }
        for (int i = 0; i < n; ++i) {
          double x = mesh.vertices()[i].x();
          double y = mesh.vertices()[i].y();
          mesh.addVertex(x, y, h);
        }

        for (int i = 0; i < n; ++i)
          mesh.addFace(i, (i + 1) % n, n + (i + 1) % n, n + i);

        std::vector<int> bottom, top;
        for (int i = 0; i < n; ++i) {
          bottom.push_back(n - i - 1);
          top.push_back(2 * n - i - 1);
        }
        mesh.addFace(bottom);
        mesh.addFace(top);

        renderMesh(mesh, color, T, lines);

        return true;
      }

      bool renderSphere(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        int n;
        GFX::Color color;

        try {
          n = conf[figureName]["n"];
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }
 
        double sqrt_5_2 = std::sqrt(5.0) / 2.0;
        double pi_2_5 = 2.0 * M_PI / 5.0;
        double pi_5 = M_PI / 5.0;

        std::vector<GFX::vec3> verts0;
        verts0.push_back(GFX::vec3(0.0, 0.0, sqrt_5_2));
        verts0.push_back(GFX::vec3(std::cos(0.0 * pi_2_5), std::sin(0.0 * pi_2_5), 0.5));
        verts0.push_back(GFX::vec3(std::cos(1.0 * pi_2_5), std::sin(1.0 * pi_2_5), 0.5));
        verts0.push_back(GFX::vec3(std::cos(2.0 * pi_2_5), std::sin(2.0 * pi_2_5), 0.5));
        verts0.push_back(GFX::vec3(std::cos(3.0 * pi_2_5), std::sin(3.0 * pi_2_5), 0.5));
        verts0.push_back(GFX::vec3(std::cos(4.0 * pi_2_5), std::sin(4.0 * pi_2_5), 0.5));
        verts0.push_back(GFX::vec3(std::cos(pi_5 + 0.0 * pi_2_5), std::sin(pi_5 + 0.0 * pi_2_5), -0.5));
        verts0.push_back(GFX::vec3(std::cos(pi_5 + 1.0 * pi_2_5), std::sin(pi_5 + 1.0 * pi_2_5), -0.5));
        verts0.push_back(GFX::vec3(std::cos(pi_5 + 2.0 * pi_2_5), std::sin(pi_5 + 2.0 * pi_2_5), -0.5));
        verts0.push_back(GFX::vec3(std::cos(pi_5 + 3.0 * pi_2_5), std::sin(pi_5 + 3.0 * pi_2_5), -0.5));
        verts0.push_back(GFX::vec3(std::cos(pi_5 + 4.0 * pi_2_5), std::sin(pi_5 + 4.0 * pi_2_5), -0.5));
        verts0.push_back(GFX::vec3(0.0, 0.0, -sqrt_5_2));
       
        std::vector<GFX::Mesh::Face> faces0;
        faces0.push_back(GFX::Mesh::make_face(0, 1, 2));
        faces0.push_back(GFX::Mesh::make_face(0, 2, 3));
        faces0.push_back(GFX::Mesh::make_face(0, 3, 4));
        faces0.push_back(GFX::Mesh::make_face(0, 4, 5));
        faces0.push_back(GFX::Mesh::make_face(0, 5, 1));
        faces0.push_back(GFX::Mesh::make_face(1, 6, 2));
        faces0.push_back(GFX::Mesh::make_face(2, 6, 7));
        faces0.push_back(GFX::Mesh::make_face(2, 7, 3));
        faces0.push_back(GFX::Mesh::make_face(3, 7, 8));
        faces0.push_back(GFX::Mesh::make_face(3, 8, 4));
        faces0.push_back(GFX::Mesh::make_face(4, 8, 9));
        faces0.push_back(GFX::Mesh::make_face(4, 9, 5));
        faces0.push_back(GFX::Mesh::make_face(5, 9, 10));
        faces0.push_back(GFX::Mesh::make_face(5, 10, 1));
        faces0.push_back(GFX::Mesh::make_face(1, 10, 6));
        faces0.push_back(GFX::Mesh::make_face(11, 7, 6));
        faces0.push_back(GFX::Mesh::make_face(11, 8, 7));
        faces0.push_back(GFX::Mesh::make_face(11, 9, 8));
        faces0.push_back(GFX::Mesh::make_face(11, 10, 9));
        faces0.push_back(GFX::Mesh::make_face(11, 6, 10));

        std::vector<GFX::vec3> verts1;
        std::vector<GFX::Mesh::Face> faces1;

        for (int i = 0; i < n; ++i) {
          verts1.clear();
          faces1.clear();

          for (std::size_t j = 0; j < faces0.size(); ++j) {
            const GFX::Mesh::Face &face = faces0[j];

            GFX::vec3 A(verts0[face[0]]);
            GFX::vec3 B(verts0[face[1]]);
            GFX::vec3 C(verts0[face[2]]);

            GFX::vec3 D((A + B) / 2.0);
            GFX::vec3 E((A + C) / 2.0);
            GFX::vec3 F((B + C) / 2.0);

            int v = verts1.size();

            verts1.push_back(A);
            verts1.push_back(B);
            verts1.push_back(C);
            verts1.push_back(D);
            verts1.push_back(E);
            verts1.push_back(F);

            faces1.push_back(GFX::Mesh::make_face(v    , v + 3, v + 4));
            faces1.push_back(GFX::Mesh::make_face(v + 1, v + 5, v + 3));
            faces1.push_back(GFX::Mesh::make_face(v + 2, v + 4, v + 5));
            faces1.push_back(GFX::Mesh::make_face(v + 3, v + 5, v + 4));
          }

          verts1.swap(verts0);
          faces1.swap(faces0);
        }
        
        for (std::size_t i = 0; i < verts0.size(); ++i)
          verts0[i].normalize();
        
        GFX::Mesh mesh;

        for (std::size_t i = 0; i < verts0.size(); ++i)
          mesh.addVertex(verts0[i]);
        for (std::size_t i = 0; i < faces0.size(); ++i)
          mesh.addFace(faces0[i]);
        
        renderMesh(mesh, color, T, lines);

        return true;
      }

      int columnMajorIndex(int row, int col, int cols)
      {
        return row * cols + col;
      }

      bool renderTorus(const std::string &figureName, const ini::Configuration &conf,
          GFX::Lines2D &lines, const GFX::mat4 &T)
      {
        int n;
        int m;
        double r;
        double R;
        GFX::Color color;

        try {
          n = conf[figureName]["n"];
          m = conf[figureName]["m"];
          R = conf[figureName]["R"];
          r = conf[figureName]["r"];
          color = extractColor(conf[figureName]["color"]);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return false;
        }
        
        GFX::Mesh mesh;

        for (int i = 0; i < n; ++i)
          for (int j = 0; j < m; ++j) {
            double u = 2.0 * i * M_PI / n;
            double v = 2.0 * j * M_PI / m;
            double x = (R + r * std::cos(v)) * std::cos(u);
            double y = (R + r * std::cos(v)) * std::sin(u);
            double z = r * std::sin(v);
            mesh.addVertex(x, y, z);

            int p0 = columnMajorIndex(i          , j          , m);
            int p1 = columnMajorIndex((i + 1) % n, j          , m);
            int p2 = columnMajorIndex((i + 1) % n, (j + 1) % m, m);
            int p3 = columnMajorIndex(i          , (j + 1) % m, m);
            mesh.addFace(p0, p1, p2, p3);
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
        
        GFX::Lines2D lines;

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
              if (!renderCube(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "Tetrahedron") {
              if (!renderTetrahedron(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "Octahedron") {
              if (!renderOctahedron(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "Icosahedron") {
              if (!renderIcosahedron(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "Dodecahedron") {
              if (!renderDodecahedron(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "Cone") {
              if (!renderCone(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "Cylinder") {
              if (!renderCylinder(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "Sphere") {
              if (!renderSphere(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "Torus") {
              if (!renderTorus(figureName, conf, lines, project * model))
                return img::EasyImage();
            } else if (type == "3DLSystem") {
              std::string inputfile = conf[figureName]["inputfile"].as_string_or_die();
              renderMesh(*LSystem3D::generateMesh(inputfile), color, project * model, lines);
            }

          } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return img::EasyImage();
          }
        }

        return draw_lines(lines, size, img::Color(255 * bgColor.r, 255 * bgColor.g, 255 * bgColor.b));
      }

  };

  PLUGIN_FACTORY("Wireframe", Wireframe);

}
