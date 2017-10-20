======================
sphinxcontrib-imagesvg
======================

This package contains the imagesvg Sphinx extension.
The extension defines a directive, "imagesvg", for embedding an svg image
in a specified HTML tag.

Install
=======

.. code-block:: bash

   > sudo pip install sphinxcontrib-imagesvg


Usage
=====

Enable the extension
--------------------

To enable this extension, add ``sphinxcontrib.imagesvg`` module to extensions
option in conf.py:

.. code-block:: python


   # Enabled extensions
   extensions = ['sphinxcontrib.imagesvg']


Example
-------

Code::

    .. imagesvg:: test.svg
       :tagtype: object

Options
-------

``tagtype``
  Specify the type of HTML tag in which you want to embed your image. Currently, we
  support 'object' and 'iframe'. Default: 'object'

All options from the ``image`` directive can be used (e.g. width, scale, target...)

Background
==========

From the perspective of HTML rendering, SVG images differ from regular images, in the following ways:

* they can be animated, whereby the animation can be triggered in various ways
* they can be styled.

The availability of these features depends on the way in which the svg is embedded into the HTML code. We have multiple ways of doing that:

* Copy/paste SVG code within HTML code (inlining)
* Using the HTML img tag
* Using the HTML object tag
* Using the HTML iframe tag
* Including SVG within SVG using the image tag.
* Using CSS (background images)

The Sphinx `image` directive will embed an svg image using the HTML img tag. As a result, the interaction feature is not available.

