.. SWF Sphinx Extension documentation master file, created by
   sphinx-quickstart on Wed Sep 26 00:40:29 2012.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to SWF Sphinx Extension's documentation!
================================================

.. default-domain:: rst

.. directive:: swf

   This directive is for improved SWF handling. If you pass a relative
   path, it is assumed it is a local file, if you pass a complete URL, 
   it is assumed it is an internet resource::

       .. swf:: path/to/some.swf

   You may pass any parameter, which is found at `Flash Help Page`_.

   .. _Flash Help Page:
      http://helpx.adobe.com/flash/kb/flash-object-embed-tag-attributes.html

   Apart from this you can pass `class`, `width`, `height` or `zoom-to-fit`.

   There are sometimes flashvideos, where you do not know the exact size.
   For this issue there is `zoom-to-fit` parameter, which will zoom out/in the
   video to fit into your rectangle specified by `width` and `height`::

       .. swf:: path/to/some.swf
          :width: 400
          :height: 300
          :zoom-to-fit: true

.. confval:: swf_zoom_to_fit_default

   Default is "yes", may also be "no"

.. confval:: swf_allowfullscreen_default

   Default is "yes", may also be "no"

.. confval:: swf_width_default

   Default is "400"

.. confval:: swf_height_default

   Default is "300"


Changes
=======

=== ======================================================================
0.3 make docutils swf node inline, such that substitutions work
--- ----------------------------------------------------------------------
0.2 add class "sphinxcontrib-swf" to container element
=== ======================================================================