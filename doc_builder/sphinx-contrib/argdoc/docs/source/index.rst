welcome to `sphinxcontrib-argdoc`
=================================

Introduction
------------
:obj:`sphinxcontrib.argdoc` is an extension for the `Sphinx`_ documentation engine.

It automatically generates tables describing command-line arguments
for :term:`executable scripts` written in `Python`_, and inserts
those tables into the scripts' ``:automodule:`` documentation. The
only requirements are:

 1. The exectuable scripts use the Python :mod:`argparse` module for
    argument parsing
 
 2. The ``rst`` stub files for the scripts include the ``:automodule:``
    directive (which they will, by default, if you use `sphinx-apidoc`_).

To start using :obj:`sphinxcontrib.argdoc`, see the :doc:`quickstart` guide.
   
To view the source code, check out `the sphinx-contrib repository on BitBucket <http://bitbucket.org/birkenfeld/sphinx-contrib>`_.

Indices and tables
------------------

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

