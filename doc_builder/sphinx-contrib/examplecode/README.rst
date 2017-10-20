.. -*- restructuredtext -*-

================================
examplecode extension for Sphinx
================================

:author: Serge Domkowski <sergedomk@gmail.com>


About
=====

This is a simple extension that, when rendered as HTML, will fold multiple
code blocks containing different programming languages into a single block
which can be toggled from one to another using buttons.

It's intended to be used for displaying example code in multiple languages
(e.g., client code for accessing an API).

This extension adds the ``example-code`` directive which adds a class to
a container wrapping the code blocks that should be folded. The class allows
the included Javascript and CSS to render the folded block and buttons.

Quick Example
-------------

Source would look something like this::

    .. example-code::
        .. code-block:: python

            import my-api

        .. code-block:: ruby

            require 'my-api'

Install
=======

From source (tar.gz or checkout)
--------------------------------

Unpack the archive, enter the sphinxcontrib-examplecode directory and run::

    python setup.py install

Enabling the extension in Sphinx_
---------------------------------

Just add ``sphinxcontrib.examplecode`` to the list of extensions in the
``conf.py`` file. For example::

    extensions = ['sphinxcontrib.examplecode']

TODO
====

* Add color configuration options for buttons.

.. Links:
.. _Sphinx: http://sphinx.pocoo.org/`

