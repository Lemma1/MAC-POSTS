Getting Started
======================

Operating System
---------------------

We support Mac OS and linux system.


Download the codes
---------------------

The codes are public and stored in `GitHub <https://github.com/Lemma1/MINAMI>`_, you can use the following line to download the code to your current directory.

.. code-block:: sh

  git clone https://github.com/Lemma1/MINAMI.git


Requirements
------------------

The package is self-contained, so these's no additional package you need to install before the installation, but we do have requirement on the compilier and code manager.

.. hlist::
  :columns: 1

  * g++, should support C++ 11.
  * cmake 2.8+, cmake can be easily installed by apt-get in Ubuntu and GUI installation process in Mac OS. For other OS, please refer to `cmake download <https://cmake.org/download/>`_.


Install
------------

To install the library, first go to the directory of the cloned folder, say /some/path/MINAMI, then excute

.. code-block:: sh

  mkdir build

to build a folder for makefiles and then excute

.. code-block:: sh

  ./build.sh

The excution will be fast and after the excution, you will find a bunch of files in build folder, then run

.. code-block:: sh

  cd build && make

After several minutes, the library is installed.


What to compile
------------------

We strongly recommend you to read the basic documents for CMake. Basically, you can choose what to compile through cmake, and in our codes, the basic libraries will always be compiled, but the examples can be compiled by your choice. The selection can be found in

.. code-block:: sh

  MINAMI/src/examples/CMakeLists.txt

.. toctree::
   :maxdepth: 1


* `Home <index.html>`_
* :ref:`genindex`
* :ref:`search`