Computer Graphics
=================

Build Instructions for Engine
-----------------------------

There is a Makefile in the top level directory. To build, dimply type make.

$ make

Additional *.ini Files
----------------------

* 2D L-System plant (not stochastic)

  data/plant.ini
  data/plant.L2D

* 2D L-System stochastic plant

  data/stochastic_plant.ini
  data/stochastic_plant.L2D

  The probability of a replacement rule is specified in the L2D file as
  illustrated below:

      Rules = {
              X -> "F-((X)+X)+F(+FX)-X",
              F -> 0.5 "FF",
              F -> 0.5 "FFF"
      }

  The probabilities of a replacement rule must sum to 1.0.

Graphics Library: libgfx
------------------------

Dependencies
^^^^^^^^^^^^

Eigen3
~~~~~~

The library depends on Eigen3, a header-only matrix library using expression
templates for efficiency. This was needed since the provided Vector3D/Matrix
classes did not correctly handle the W component of the homogeneous
coordinates needed for the perspective divide in the rendering pipeline.
The library source is embedded inside the tgz archive and the include paths
are set so this dependency does not have to be downloaded separately.

Qt4
~~~

The Qt4 libraries are required to be installed on the system in order to build
the GUI example. CMake will detect Qt4 and build the GUI example if available.

Building Instructions
~~~~~~~~~~~~~~~~~~~~~

The GUI example can only be build using CMake. Both CMake and Qt4 are installed
on the computers in room G0.25.

$ mkdir build
$ cd build
$ make
$ ./gui/gui

Features
^^^^^^^^

* Design using vertex and fragment shaders similar to OpenGL
* Draw 3D lines, triangles and quads (wireframe and shaded)
* Support for viewports (needed for interactive graphics)
* Initial support for lighting (ambient + diffuse)
* Texture mapping (including mipmapping to improve quality)
