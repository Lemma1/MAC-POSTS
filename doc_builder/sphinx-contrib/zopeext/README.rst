.. -*- rst -*- -*- restructuredtext -*-

==================
zopeext for Sphinx
==================

:author: Michael McNeil Forbes <mforbes@alum.mit.edu>

This extension provides an :rst:dir:`autointerface` directive for `Zope
interfaces`_.

Requirements
============

* Sphinx_: ``pip install sphinx``
* zope.interface_: ``pip install zope.interface``
* sphinxcontrib.zopeext_: ``pip install sphinxcontrib-zopeext``

Usage
=====

In the `build configuration file`_ (the ``conf.py`` in your Sphinx_
documentation directory) add :mod:`sphinxcontrib.zopeext.autointerface` to your
``extensions`` list::

   extensions = [..., 'sphinxcontrib.zopeext.autointerface', ...]


Then, in your documentation, use :rst:dir:`autointerface` as you would use
:rst:dir:`autoclass`.  Here is an example (click on the "[source]" link at the
right to see the code):

.. autointerface:: sphinxcontrib.zopeext.example.IMyInterface

.. note:: We have included the ``autointerface.css`` which simply adds the
   following rule to give a green background for the interface::

      dl.interface > dt { background-color: #33FF33; }

.. _Sphinx: http://sphinx.pocoo.org/
.. _build configuration file: http://sphinx.pocoo.org/config.html
.. _Zope interfaces: http://docs.zope.org/zope.interface/README.html
.. _zope.interface: http://pypi.python.org/pypi/zope.interface/
.. _sphinxcontrib.zopeext: http://pypi.python.org/pypi/sphinxcontrib-zopeext/
