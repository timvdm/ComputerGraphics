#include "utils.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdlib>

#include "plugin.h"

img::EasyImage generate_image(const ini::Configuration &conf)
{
  std::string type;
  try {
    type = conf["General"]["type"].as_string_or_die();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return img::EasyImage();
  }

  CG::Plugin *plugin = CG::Plugin::findPlugin(type);
  if (!plugin) {
    std::cerr << "Could not find plugin for type: " << type << std::endl;
    return img::EasyImage();
  }

  return plugin->image(conf);
}

int main(int argc, char const* argv[])
{
        srand(time(NULL));

        CG::Plugin::setFilename(argv[1]);

        int retVal = 0;
        try
        {
                for(int i = 1; i < argc; ++i)
                {
                        ini::Configuration conf;
                        try
                        {
                                std::ifstream fin(argv[i]);
                                fin >> conf;
                                fin.close();
                        }
                        catch(ini::ParseException& ex)
                        {
                                std::cerr << "Error parsing file: " << argv[i] << ": " << ex.what() << std::endl;
                                retVal = 1;
                                continue;
                        }

                        img::EasyImage image = generate_image(conf);
                        if(image.get_height() > 0 && image.get_width() > 0)
                        {
                                std::string fileName(argv[i]);
                                std::string::size_type pos = fileName.rfind('.');
                                if(pos == std::string::npos)
                                {
                                        //filename does not contain a '.' --> append a '.bmp' suffix
                                        fileName += ".bmp";
                                }
                                else
                                {
                                        fileName = fileName.substr(0,pos) + ".bmp";
                                }
                                try
                                {
                                        std::ofstream f_out(fileName.c_str(),std::ios::trunc | std::ios::out);
                                        f_out << image;

                                }
                                catch(std::exception& ex)
                                {
                                        std::cerr << "Failed to write image to file: " << ex.what() << std::endl;
                                        retVal = 1;
                                }
                        }
                        else
                        {
                                std::cout << "Could not generate image for " << argv[i] << std::endl;
                        }
                }
        }
        catch(const std::bad_alloc &exception)
        {
    		//When you run out of memory this exception is thrown. When this happens the return value of the program MUST be '100'.
    		//Basically this return value tells our automated test scripts to run your engine on a pc with more memory.
    		//(Unless of course you are already consuming the maximum allowed amount of memory) 
    		//If your engine does NOT adhere to this requirement you risk losing points because then our scripts will 
		//mark the test as failed while in reality it just needed a bit more memory
                std::cerr << "Error: insufficient memory" << std::endl;
                retVal = 100;
        }
        return retVal;
}
