====================
swf Sphinx Extension
====================

This package contains the swf Sphinx extension.

It adds a directive `swf` for improved flash file handling.

You can pass any parameter mentioned on `flash attribute listing`_.

.. _flash attribute listing:
   http://helpx.adobe.com/flash/kb/flash-object-embed-tag-attributes.html

I stumbled across the issue, that screencasts often are "zoomed in", such
that you do not have access to controls. So this directive has a
"zoom-to-fit" option, which zooms out the video using javascript.

Example::

    .. swf:: path/to/your.swf
       :zoom-to-fit: true


Installation
------------

For install type::

    pip install sphinxcontrib-swf


Enable Sphinx Extension
-----------------------

Add ``sphinxcontrib.swf`` to your ``extensions`` in `conf.py`::

    extensions = [ 'sphinxcontrib.swf' ]


Author
------

`Kay-Uwe (Kiwi) Lorenz <kiwi@franka.dyndns.org>`_ (http://quelltexter.org)

