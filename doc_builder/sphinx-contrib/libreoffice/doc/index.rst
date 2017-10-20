.. -*- restructuredtext -*-

================================
LibreOffice extension for Sphinx
================================

:author: Gerard Marull-Paretas <gerardmarull@gmail.com>

About
=====

This extensions allows to render any supported LibreOffice drawing format 
(e.g. odg, vsd...) 


Install
=======

First of all, make sure to have LibreOffice_ installed.

.. warning::
    LibreOffice 4.3 is known to **NOT** work. Use other versions instead.

You can install it from PyPI::

    $ pip install sphinxcontrib-libreoffice

Alternatively, you can grab it from the ``sphinx-contrib`` repository::

    $ hg clone https://bitbucket.org/birkenfeld/sphinx-contrib/
    $ cd sphinx-contrib/libreoffice
    $ python setup.py install


Usage
=====

Enabling the extension
----------------------

Just add ``sphinxcontrib.libreoffice`` to the list of extensions in the 
``conf.py`` file. For example::

    extensions = ['sphinxcontrib.libreoffice']

Quick example
-------------

A quick example:

.. libreoffice:: test.odg
   :align: center
   :autocrop:

   The optional caption

Code::

    .. libreoffice:: test.odg
       :align: center
       :autocrop:

       The optional caption

Options
-------

``autocrop``
  Remove empty margins from the rendered drawings (image formats only)

All options from ``figure`` directive can be used (e.g. scale, target...)

Configuration
-------------

Two optional configurations are added to Sphinx_. They can be set in
``conf.py`` file:

``libreoffice_fromat`` <dict>:
  image format used for the different builders. ``latex`` and ``html`` fromats
  are supported.

  For example::

    libreoffice_format = dict(latex='pdf', html='png')

  These are the actual defaults.

``libreoffice_binary`` <str>:
  path to the LibreOffice binary (soffice). LibreOffice binary path will be 
  automatically determined by the extension. Use only if you need to indicate a 
  specific version or you have it installed in a custom path.

.. Links:
.. _LibreOffice: http://www.libreoffice.org/
.. _Sphinx: http://sphinx-doc.org/

.. include:: ../CHANGES.rst

