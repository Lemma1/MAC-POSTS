slide extension README
=======================

This is a sphinx extension for embedding your presentation slides.

This extension enable you to embed your slides on slideshare_ and other sites.
Following code is sample::

   .. slide:: http://www.slideshare.net/TakeshiKomiya/blockdiag-a-simple-diagram-generator


.. _slideshare: http://www.slideshare.net/


Setting
=======

.. You can see available package at `PyPI <http://pypi.python.org/pypi/sphinxcontrib-slide>`_.

You can get archive file at http://bitbucket.org/birkenfeld/sphinx-contrib/

Install
-------

.. code-block:: bash

   > easy_install sphinxcontrib-slide


Configure Sphinx
----------------

To enable this extension, add ``sphinxcontrib.slide`` module to extensions 
option at :file:`conf.py`. 

.. code-block:: python

   import os, sys

   # Path to the folder where blockdiag.py is
   # NOTE: not needed if the package is installed in traditional way
   # using setup.py or easy_install
   sys.path.append(os.path.abspath('/path/to/sphinxcontrib.slide'))

   # Enabled extensions
   extensions = ['sphinxcontrib.slide']


Directive
=========

.. describe:: .. slide:: [URL]

   This directive insert slide interface into the generated document.
   sphinxcontrib-slide supports presentations on slideshare, googledocs, speakerdeck and slides.com.

   Examples::

      .. slide:: http://www.slideshare.net/TakeshiKomiya/blockdiag-a-simple-diagram-generator
