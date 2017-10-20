=============================
FindAnything Sphinx Extension
=============================

This extension enables you to have a `Sublime Text 2`_ like findanything
panel on your page, letting you search as you type for pages, sections
and index entries.

.. _Sublime Text 2: http://sublimetext.com


Installation
------------

::

    pip install sphinxcontrib-findanything


Enable Extension
----------------

Add ``sphinxcontrib.findanything`` in ``extensions`` of `conf.py`::

    extensions = [ 'sphinxcontrib.findanything' ]


Configuration
-------------

``html_findanything_css``
    Set this value to a filename accessible under `_static/` folder or
    **None**, if you want to disable the default css file (e.g. if you
    setup the styling in a theme).  Default is ``findanything.css``.

``html_findanything_width``
    Default is ``400px``.

``html_findanything_add_indexentries``
    Default is **True**.  Make index entries available for finding.

``html_findanything_add_topics``
    Default is **True**.  Make page titles and sections available for finding.

``html_findanything_use_cached_hits``
    Default is **True**.  Boost already visited pages via findanything panel
    in next searches to the top.

``html_findanything_hit_cacher``
    Default is ``cookie``. And this is the only possible value. There is 
    no other backend for caching hits.


Future
------

* make it configurable if use the path from link or create path from 
  toctree


Author
------

`Kay-Uwe (Kiwi) Lorenz <kiwi@franka.dyndns.org>`_ (http://quelltexter.org)
