# determine g++ version
#   4.6: use -std=c++0x
#   4.7: use -std=c++11
GXXVERSION := $(shell expr `$(CXX) -dumpversion`)
ifeq ($(GXXVERSION),4.7)
  CXXSTD = -std=c++11
else
  CXXSTD = -std=c++0x
endif

FLAGS = -c -O2 -pedantic -Wall -Wno-reorder -Wno-sign-compare -Wno-enum-compare -Ithirdparty/eigen3 -I. $(CXXSTD)

all: engine
#	echo $(FLAGS)

########################################
#
# engine command line tool
#
########################################

engine: engine.o EasyImage.o ini_configuration.o lparser.o render.o LineDrawing.o LSystem2D.o LSystem3D.o Wireframe.o ZBufferedWireframe.o ZBuffering.o LightedZBuffering.o  transform.o mesh.o texture.o 
	$(CXX) engine.o EasyImage.o ini_configuration.o lparser.o render.o LineDrawing.o LSystem2D.o LSystem3D.o Wireframe.o ZBufferedWireframe.o ZBuffering.o LightedZBuffering.o transform.o mesh.o texture.o -o engine

engine.o: src/engine.cc
	$(CXX) $(FLAGS) src/engine.cc

########################################
#
# Utilities provided by assistant
#
########################################

EasyImage.o: src/utils/EasyImage.h src/utils/EasyImage.cc
	$(CXX) $(FLAGS) src/utils/EasyImage.cc

ini_configuration.o: src/utils/ini_configuration.hh src/utils/ini_configuration.cc
	$(CXX) $(FLAGS) src/utils/ini_configuration.cc

lparser.o: src/utils/lparser.h src/utils/lparser.cc
	$(CXX) $(FLAGS) src/utils/lparser.cc


########################################
#
# Code to handle assignment specifics
#
########################################

render.o: src/labo/render.h src/labo/render.cpp
	$(CXX) $(FLAGS) src/labo/render.cpp

LineDrawing.o: src/labo/LineDrawing.cpp
	$(CXX) $(FLAGS) src/labo/LineDrawing.cpp

LSystem2D.o: src/labo/LSystem2D.cpp
	$(CXX) $(FLAGS) src/labo/LSystem2D.cpp

LSystem3D.o: src/labo/LSystem3D.cpp
	$(CXX) $(FLAGS) src/labo/LSystem3D.cpp

Wireframe.o: src/labo/Wireframe.cpp
	$(CXX) $(FLAGS) src/labo/Wireframe.cpp

ZBufferedWireframe.o: src/labo/ZBufferedWireframe.cpp
	$(CXX) $(FLAGS) src/labo/ZBufferedWireframe.cpp

ZBuffering.o: src/labo/ZBuffering.cpp
	$(CXX) $(FLAGS) src/labo/ZBuffering.cpp

LightedZBuffering.o: src/labo/LightedZBuffering.cpp
	$(CXX) $(FLAGS) src/labo/LightedZBuffering.cpp

########################################
#
# libgfx library
#
########################################

transform.o: libgfx/transform.h libgfx/transform.cpp
	$(CXX) $(FLAGS) libgfx/transform.cpp

mesh.o: libgfx/mesh.h libgfx/mesh.cpp
	$(CXX) $(FLAGS) libgfx/mesh.cpp

texture.o: libgfx/texture.h libgfx/texture.cpp
	$(CXX) $(FLAGS) libgfx/texture.cpp

########################################
#
# Clean
# 
########################################

clean:
	rm -rf *.o
	rm -rf engine
