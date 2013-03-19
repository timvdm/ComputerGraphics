#include "../utils.h"
#include "../plugin.h"
#include "../libgfx/utility.h"

#include "render.h"

#include <fstream>
#include <cmath>

namespace CG {

  class LSystem2D : public Plugin
  {
    public:
      img::EasyImage image(const ini::Configuration &conf)
      {
        int size;
        img::Color bgColor;
        std::vector<double> lineColor;
        std::string filename;

        try {
          size = conf["General"]["size"];
          bgColor = extractColor(conf["General"]["backgroundcolor"]);
          lineColor = conf["2DLSystem"]["color"];
          filename = conf["2DLSystem"]["inputfile"].as_string_or_die();
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return img::EasyImage();
        }

        // parse L2D file
        LParser::LSystem2D lSystem;
        std::ifstream ifs(filename.c_str());
        assert(ifs);
        try {
          ifs >> lSystem;
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
          return img::EasyImage();
        }
        ifs.close();

        // convert LSystem2D to set of lines
        GFX::Lines2D lines;
        drawLSystem(lSystem, lines);

        // set the line color
        for (std::size_t i = 0; i < lines.size(); ++i)
          lines[i].color = GFX::Color(lineColor);


        return draw_lines(lines, size, bgColor);
      }

    private:
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

      struct LSystemState
      {
        LSystemState(double startingAngle) : angle(startingAngle)
        {
        }

        void push()
        {
          stack.push_back(std::make_pair(angle, pos));
        }

        void pop()
        {
          angle = stack.back().first;
          pos = stack.back().second;
          stack.pop_back();
        }

        double angle;
        GFX::Point2D pos;
        std::vector<std::pair<double, GFX::Point2D> > stack;
      };

      void drawLSystem(const LParser::LSystem2D &lSystem, const std::string &commands, GFX::Lines2D &lines, int iteration, LSystemState &state)
      {
        if (iteration > 0) {
          // recursive calls...
          for (std::size_t i = 0; i < commands.size(); ++i)
            switch (commands[i]) {
              case '-':
                state.angle -= GFX::deg2rad(lSystem.get_angle());
                break;
              case '+':
                state.angle += GFX::deg2rad(lSystem.get_angle());
                break;
              case '(':
                state.push();
                break;
              case ')':
                state.pop();
                break;
              default:
                drawLSystem(lSystem, lSystem.get_replacement(commands[i]), lines, iteration - 1, state);
            }
        } else {
          // reached deepest recursive level -> draw the lines
          for (std::size_t i = 0; i < commands.size(); ++i) {
            switch (commands[i]) {
              case '-':
                state.angle -= GFX::deg2rad(lSystem.get_angle());
                break;
              case '+':
                state.angle += GFX::deg2rad(lSystem.get_angle());
                break;
              case '(':
                state.push();
                break;
              case ')':
                state.pop();
                break;
              default:
                {
                  // compute new position
                  GFX::Point2D newPos(state.pos.x + std::cos(state.angle), state.pos.y + std::sin(state.angle));
                  // draw a line?
                  if (lSystem.draw(commands[i]))
                    lines.push_back(GFX::Line2D(state.pos, newPos));
                  // move to new position
                  state.pos = newPos;
                }
                break;                
            }
          }
        }
      }

      void drawLSystem(const LParser::LSystem2D &lSystem, GFX::Lines2D &lines)
      {
        LSystemState state(GFX::deg2rad(lSystem.get_starting_angle()));
        drawLSystem(lSystem, lSystem.get_initiator(), lines, lSystem.get_nr_iterations(), state);
      }


  };

  PLUGIN_FACTORY("2DLSystem", LSystem2D)

}
