CXX = g++
FLAGS = -c -pedantic -Wall -Wno-reorder -Wno-sign-compare -Wno-enum-compare -std=c++11 -Ithirdparty/eigen3

all: engine

########################################
#
# engine command line tool
#
########################################

engine: engine.o EasyImage.o ini_configuration.o lparser.o render.o LineDrawing.o LSystem2D.o LSystem3D.o Wireframe.o transform.o mesh.o
	$(CXX) engine.o EasyImage.o ini_configuration.o lparser.o render.o LineDrawing.o LSystem2D.o LSystem3D.o Wireframe.o transform.o mesh.o -o engine

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

########################################
#
# libgfx library
#
########################################

transform.o: src/libgfx/transform.h src/libgfx/transform.cpp
	$(CXX) $(FLAGS) src/libgfx/transform.cpp

mesh.o: src/libgfx/mesh.h src/libgfx/mesh.cpp
	$(CXX) $(FLAGS) src/libgfx/mesh.cpp

########################################
#
# Clean
# 
########################################

clean:
	rm -rf *.o
	rm -rf engine
