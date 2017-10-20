googlemaps extension README
============================

This is a sphinx extension for embedding maps.

This extension enable you to embed maps using `Google Maps`_ .
Following code is sample::

   .. googlemaps:: Shibuya Station


.. _Google Maps: http://maps.google.com/


Setting
=======

.. You can see available package at `PyPI <http://pypi.python.org/pypi/sphinxcontrib-googlemaps>`_.

You can get archive file at http://bitbucket.org/birkenfeld/sphinx-contrib/

Install
-------

.. code-block:: bash

   > easy_install sphinxcontrib-googlemaps


Configure Sphinx
----------------

To enable this extension, add ``sphinxcontrib.googlemaps`` module to extensions 
option at :file:`conf.py`. 

.. code-block:: python

   import os, sys

   # Path to the folder where sphinxcontrib/googlemaps.py is
   # NOTE: not needed if the package is installed in traditional way
   # using setup.py or easy_install
   sys.path.append(os.path.abspath('/path/to/sphinxcontrib.googlemaps'))

   # Enabled extensions
   extensions = ['sphinxcontrib.googlemaps']


Directive
=========

.. describe:: .. googlemaps:: [location string]

   This directive insert maps into the generated document.

   Examples::

      .. googlemaps:: Shibuya


   You can specify target point by latitude and longitue.

   Example::

      .. googlemaps::
         :latitude: 35.663991
         :longtitude: 139.730988

   googlemaps directive supports these options:

   :balloon: Show ballon to marker
   :zoom: Zoom in/out maps (default value is 15)
