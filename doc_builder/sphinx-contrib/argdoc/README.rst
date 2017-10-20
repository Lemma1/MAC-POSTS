Welcome to `sphinxcontrib-argdoc`
=================================

Introduction
------------

`sphinxcontrib-argdoc <http://sphinxcontrib-argdoc.readthedocs.org>`_
is an extension for the `Sphinx <https://sphinx-doc.org>`_
documentation engine.

It automatically generates tables describing command-line arguments
for executable scripts written in Python, and inserts those tables
into the ``:automodule:`` documentation generated for the scripts by
the Sphinx extension `autodoc <http://sphinx-doc.org/ext/autodoc.html>`_.

The only requirements are:

  1. The executable scripts use the Python
     `argparse <https://docs.python.org/3/library/argparse.html>`_ module
     for argument parsing.

  2. The `rst` documentation stub file for the scripts include
     the ``:automodule:`` directive (which they will, by default,
     if you use `sphinx-apidoc`).

For more info, detailed instructions, and examples, see the
`sphinxcontrib-argdoc documentation <http://sphinxcontrib-argdoc.readthedocs.org>`_.


Installation and use
--------------------

`sphinxcontrib-argdoc <http://sphinxcontrib-argdoc.readthedocs.org>`_
may be installed from `PyPI <https://pypi.python.org>`_
using `pip <https://pip.pypa.io/en/latest/installing.html>`_. Alternatively,
you can clone the development version into your ``PYTHONPATH``.

To use `sphinxcontrib-argdoc <http://sphinxcontrib-argdoc.readthedocs.org>`_,
simply add `'sphinxcontrib.argdoc'` to the list of extensions in your project's
Sphinx configuration file `conf.py <http://sphinx-doc.org/config.html>`_:

 .. code-block:: python

    # somewhere in conf.py
    extensions = ['sphinxcontrib.argdoc',
                  'another_extension',
                  'some_other_extension'
                 ]


Tests
-----

Tests are written using `nose <https://nose.readthedocs.org>`_,
and may be found in the subpackage `sphinxcontrib.argdoc.test`. To run the tests,
type from the terminal:

 .. code-block:: shell

    $ nosetests sphinxcontrib.argdoc.test


Authors
-------

  - `Joshua Griffin Dunn <joshua.g.dunn@gmail.com>`_


License
-------
`sphinxcontrib-argdoc` is licensed under the
`BSD 3-Clause License <http://opensource.org/licenses/BSD-3-Clause>`_.
