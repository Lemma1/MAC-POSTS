Inline Syntax Highlight Sphinx Extension
========================================

.. default-role:: rst

This extension enables you to use inline syntax highlighting in Sphinx.

Installation
------------

::

    pip install sphinxcontrib-inlinesyntaxhighlight


Enable Extension
----------------

Add ``sphinxcontrib.inlinesyntaxhighlight`` in ``extensions`` of `conf.py`::

    extensions = [ 'sphinxcontrib.inlinesyntaxhighlight' ]

Configuration
-------------

``inline_highlight_respect_highlight``
    Use defaults provided by ``highlight`` directive for ``code`` role.

``inline_highlight_literals``
    Highlight also normal literals like :code:```literal```


Usage
-----

1. define your own roles
   ::

      .. role:: python(code)
          :language: python

      :python:`"some" "highlighted code"`

   .. note:: I did some tests with role defining and it looks like you
      have to define roles in the beginning of a page. Otherwise Sphinx 
      throws strange errors.

2. if respect highlight
   ::

      .. highlight: python

      :code:`"some" "highlighted code"`

3. if literals are also highlighted
   ::

      .. highlight: python

      ``"some" "highlighted code"``

4. to get the normal literal behaviour use ``literal`` role::

      :literal:`normal literal`

   
License
-------

New BSD License.


Author
------

`Kay-Uwe (Kiwi) Lorenz <kiwi@franka.dyndns.org>`_ (http://quelltexter.org)
