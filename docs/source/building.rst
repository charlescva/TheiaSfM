.. _chapter-building:

======================
Building Theia Library
======================

Theia source code and documentation are hosted on `Github
<https://github.com/sweeneychris/TheiaSfM>`_ where you can always grab the latest version

.. _section-dependencies:

Dependencies
------------

Theia relies on a number of open source libraries. Luckily, most of the will be included in Ceres

1. C++11 is needed for certain functionality and added models to the stdlib. C++0x will probably work in most cases, but is not guaranteed. As such, you need a compiler that supports C++11 appropriately.

2. `CMake <http://www.cmake.org>`_ is a cross platform build system. Theia needs a relatively recent version of CMake (version 2.8.0 or better).


3. `eigen3 <http://eigen.tuxfamily.org/index.php?title=Main_Page>`_ is used extensively for doing nearly all the matrix and linear algebra operations.

4. `OpenImageIO <https://sites.google.com/site/openimageio/home>`_ is used to read and write image files. It is recommended to install version 1.6 or higher.

5. `Ceres Solver <https://code.google.com/p/ceres-solver/>`_ is a library for solving non-linear least squares problems. In particular, Theia uses it for Bundle Adjustment.

**NOTE**: Theia also depends on the following libraries, but they are included in the installation of Ceres so it is likely that you do not need to reinstall them.

6. `google-glog <http://code.google.com/p/google-glog>`_ is used for error checking and logging. Ceres needs glog version 0.3.1 or later. Version 0.3 (which ships with Fedora 16) has a namespace bug which prevents Ceres from building.

7. `gflags <http://code.google.com/p/gflags>`_ is a library for processing command line flags. It is used by some of the examples and tests.

Make sure all of these libraries are installed properly before proceeding. Improperly installing any of these libraries can cause Theia to not build.

.. _section-building:

Building
--------

Building should be equivalent on all platforms, thanks to CMake. To install Theia, simply run the following commands after you have installed the :ref:`section-dependencies`.

First, navigate to the source directory of the Theia library. Then execute the following commands:

.. code-block:: bash

 mkdir theia-build
 cd theia-build
 cmake ..
 make -j4
 make test

If all tests pass, then you are ready to install. If not all tests pass, you should examine the individual test to determine if it affects your performance. For instance, if the global SfM methods fail but you are only going to use incremental SfM, you probably do not need to worry about the failing tests. Email the mailing list if you are unsure about failing tests.

Theia can be install using the make install command

.. code-block:: bash

 make install

You can also try running the unit tests individually. The executables should be located in the bin directory of the theia-build folder.


.. _section-customizing:

Customizing the build
---------------------

It is possible to customize the build process by passing appropriate flags to
``CMake``. Use these flags only if you really know what you are doing.


#. ``-DBUILD_TESTING=OFF``: Use this flag to enable or disable building the unit tests. By default, this option is enabled.

#. ``-DBUILD_DOCUMENTATION=ON``: Turn this flag to ``ON`` to build the documentation with Theia. This option is disabled by default.

.. _section-docker:

Using a Docker Container
--------

Building and reconstruction can be performed utilizing a Docker container.  You can install docker on a variety of platforms such as Linux, Windows, Mac, Azure, AWS or a Windows Server.  This has only been testing using the Linux distribution of Docker, but the other platforms may work as well.

First, install Docker, and create a dockerfile like the following:

.. code-block:: bash

 FROM debian:jessie
 RUN apt-get update
 
 # For Compiling: build-essential
 
 RUN apt-get install -y wget build-essential cmake
 
 RUN mkdir /src
 RUN wget -O /src/eigen-3.3.1.tar.gz http://bitbucket.org/eigen/eigen/get/3.3.1.tar.gz
 RUN wget -O /src/oiio-latest.tar.gz https://github.com/OpenImageIO/oiio/tarball/master
 RUN wget -O /src/ceres-solver-1.12.tar.gz https://ceres-solver.googlesource.com/ceres-solver/+archive/029799d757b4ed2be5af64899178928f18cb6e28.tar.gz
 RUN wget -O /src/theia-latest.tar.gz https://github.com/sweeneychris/TheiaSfM/archive/v0.7.tar.gz
 
 #Make and Install Eigen
 RUN tar -C /src/ -xvf /src/eigen-3.3.1.tar.gz
 RUN mkdir /src/eigen-eigen-f562a193118d/build
 RUN cd /src/eigen-eigen-f562a193118d/build; cmake ..
 RUN cd /src/eigen-eigen-f562a193118d/build; make install
 
 #Make OIIO
 RUN tar -C /src/ -xvf /src/oiio-latest.tar.gz
 RUN apt-get install -y libtiff-dev libpng-dev libopenexr-dev libboost-regex-dev libboost-thread-dev libboost-filesystem-dev
 RUN cd /src/OpenImageIO-oiio-*; make -j4
 RUN ln -s /src/OpenImageIO-oiio-*/dist/linux64/include/OpenImageIO /usr/include/OpenImageIO;ln -s /src/OpenImageIO-oiio-*/dist/linux64/lib/* /usr/lib/
 
 #Make and Install Ceres Solver
 RUN mkdir -p /src/ceres-src/build
 RUN tar -C /src/ceres-src/ -xvf /src/ceres-solver-1.12.tar.gz
 RUN apt-get install -y libgoogle-glog-dev libsuitesparse-dev
 RUN cd /src/ceres-src/build; cmake ..; make -j4
 RUN cd /src/ceres-src/build; make install
 
 #Make & Install Theia
 RUN apt-get install -y libhdf5-dev libgflags-dev mesa-common-dev libgl1-mesa-dev freeglut3-dev libxmu-dev libxi-dev
 RUN tar -C /src/ -xvf /src/theia-latest.tar.gz
 RUN mkdir /src/TheiaSfM-0.7/theia-build
 RUN cd /src/TheiaSfM-0.7/theia-build; cmake ..
 RUN cd /src/TheiaSfM-0.7/theia-build; make -j4
 ENV PATH $PATH:/src/TheiaSfM-0.7/theia-build/bin
 CMD /bin/bash

If the build is successful you can then use the container as an interactive bash shell to test Theia, or you can mount volumes and perform a reconstruction using images on the host operating system.

This example will run 'build_reconstruction' on the images and flag file in /home/user/theia.  The resulting reconstruction file should be written to /src/working-path in order to end up on the host operating system.  Anything not written to /src/working-path will be removed when the container is finished.  You can disable this behavior by omitting the --rm=true flag, or set it to false.

.. code-block:: bash

 docker run -it --rm=true -v /home/user/theia:/src/working-path theia:latest /src/TheiaSfM-0.7/theia-build/bin/build_reconstruction --flagfile=/src/working-path/flags.txt

Make sure your flag file is also in the same path as your input set and that the paths in it reference the path inside the container, not the path on your host.  See Docker's documentation on Volumes for more information.
