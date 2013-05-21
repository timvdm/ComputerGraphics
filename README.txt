Computer Graphics
=================

Build Instructions for Engine
-----------------------------

There is a Makefile in the top level directory. To build, simply type make.

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

Texture Mapping
---------------

There are no additional ini files for texture mapping but there is an
interactive example using texture mapping described below.

Graphics Library: libgfx
------------------------

Features
^^^^^^^^

* Design using vertex and fragment shaders similar to OpenGL
** This allows all types of light and many other features like bump mapping,
   ambient occlusion, etc. to be implemented without having to change the
   rasterizer code.
* Draw 3D lines, triangles and quads (wireframe and shaded)
* Support for viewports (needed for interactive graphics)
* Texture mapping (including mipmapping to improve quality)
* Ambient occlusion (a.k.a. sky light)
* Particle systems
* Clipping is not implemented which means the camera must always be outside
  the model

Build Instructions
^^^^^^^^^^^^^^^^^^

The GUI example can only be build using CMake. Both CMake and Qt4 are installed
on the computers in room G0.25.

$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make

Examples
^^^^^^^^

* Textures

  This example shows a house with UV-mapped textures.

  Once the examples are compiled, this example can be started using:

  $ ./gui/texture

* Fire

  This example shows a particle system to create fire effects.

  Once the examples are compiled, this example can be started using:

  $ ./gui/fire

* Molecule Viewer

  This example shows a simple molecule viewer. There are some molecule files in
  the data/ directory. These are taken from the Protein Data Bank and have the
  file extension pdb. This example illustrates ambient occlusion which mimics
  skylight. This is done by computing occlusion maps from multiple (e.g. 100)
  light directions and later using these maps (i.e. textures) compute the pixel
  colors. The spheres in this example are rendered using only 2 triangles that
  always face the camera and some mathematics to create pixel perfect spheres
  at a minimal computational cost.

  This example is based on this paper:

  Marco Tarini, Paolo Cignoni, and Claudio Montani, Ambient Occlusion and Edge
  Cueing for Enhancing Real Time Molecular Visualization, IEEE Transactions on
  Visualization and Computer Graphics, Vol. 12, No. 5, 2006

  http://vcg.isti.cnr.it/Publications/2006/TCM06/Tarini_FinalVersionElec.pdf

  Once the examples are compiled, this example can be started using:

  $ ./gui/pdbviewer/pdbviewer ../data/1DRF.pdb

  Additional input files can be obtained from the Protein Data Bank:

  http://www.rcsb.org/pdb/home/home.do

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

Known Bugs
----------

* shadowing267.ini & shadowing268.ini

  The epsilon value for determining if a pixel is lit is set differently for these
  two cases. If the usual value is used, artifacts can be seen in these two cases.

* Thick 3D L-Systems

  There are more spheres in the 3D L-Systems compared to the reference images. At
  first it seems the reference images don't include a sphere when the line does
  not change direction but this is not always the case.
