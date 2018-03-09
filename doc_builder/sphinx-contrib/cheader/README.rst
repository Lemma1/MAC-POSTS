*********************
sphinxcontrib-cheader
*********************

This is a Sphinx_ extension that implements the ``c:header`` directive that
is inadvertently absent from Sphinx's built-in `C domain`_. It works
similarly to the built-in ``py:module`` directive.

.. _Sphinx:   http://sphinx-doc.org/
.. _C domain: http://sphinx-doc.org/domains.html#the-c-domain

Examples
========

At the beginning of the description of a header file in your documentation,
insert a corresponding ``c:header`` directive as follows::

   .. c:header:: <libfoobar.h>

The aforementioned creates an index entry of: `libfoobar.h (C header)`.

For a real usage example, see the http://libcpr.org documentation.
The corresponding Sphinx sources can be found at:
https://github.com/dryproject/libcpr/tree/master/doc/manual

Installation
============

Refer to the `PyPI Tutorial`_ for installation instructions. If you have a
modern Python environment, installation should be as easy as::

   $ sudo pip install sphinxcontrib-cheader

.. _PyPI Tutorial: https://wiki.python.org/moin/CheeseShopTutorial

Configuration
=============

To enable the extension, edit your Sphinx ``conf.py`` configuration file to
include the extension name in the ``extensions`` list; for example::

   extensions = ['sphinxcontrib.cheader']

Author
======

`Arto Bendiken <https://gratipay.com/bendiken/>`_ - http://ar.to/

License
=======

This is free and unencumbered public domain software. For more information,
see http://unlicense.org/ or the accompanying ``LICENSE`` file.
