sphinxcontrib-cacoo
====================
This package contains the cacoo Sphinx extension.

This extension enable you to embed diagrams on cacoo_ .
Following code is sample::

  .. cacoo-image:: [diagramid]

  .. cacoo-figure:: [diagramid]

     caption of figure

.. _cacoo: https://cacoo.com/

Setting
=======

Install
-------

::

   $ pip install sphinxcontrib-cacoo


Configure Sphinx
----------------

Add ``sphinxcontrib.cacoo`` to ``extensions`` at `conf.py`::

   extensions += ['sphinxcontrib.cacoo']

And set your API key to ``cacoo_apikey``::

   cacoo_apikey = 'your apikey'


Directive
=========

`.. cacoo-image:: [URL]`

  This directive insert a diagram into the document.
  If your diagram has multiple sheets, specify sheetid after ``#``.

  Examples::

    .. cacoo-image:: https://cacoo.com/diagrams/EWHRuF5Kox1AnyNL

    .. cacoo-image:: https://cacoo.com/diagrams/mb53vvmYG38QGUPf#37D74

  Options are same as `image directive`_ .

`.. cacoo-figure:: [URL]`

  This directive insert a diagram and its caption into the document.

  Examples::

    .. cacoo-figure:: https://cacoo.com/diagrams/EWHRuF5Kox1AnyNL

       Structure of this system

  Options are same as `figure directive`_ .

.. _image directive: http://docutils.sourceforge.net/docs/ref/rst/directives.html#image
.. _figure directive: http://docutils.sourceforge.net/docs/ref/rst/directives.html#figure

Configuration Options
======================

cacoo_apikey

  API key for cacoo_ 


Repository
==========

This code is hosted by Bitbucket.

  http://bitbucket.org/birkenfeld/sphinx-contrib/
