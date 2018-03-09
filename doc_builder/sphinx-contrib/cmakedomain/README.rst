GNU Make Domain
===============

This `Sphinx extension`_ provides a `CMake`_ `domain`_.

.. _Sphinx extension: http://sphinx-doc.org
.. _GNU Make: http://cmake.org
.. _domain: http://sphinx-doc.org/domains.html


Installation
------------

::

    pip install sphinxcontrib-cmakedomain


Enable Extension
----------------

Add ``sphinxcontrib.cmakedomain`` in ``extensions`` of `conf.py`::

    extensions = [ 'sphinxcontrib.cmakedomain' ]


Usage
-----

Get ready for make::

    .. highlight:: cmake

    .. default-domain:: make

Describe a macro::

    .. macro:: foo(SOME <var> ANOTHER <var2>)

       :param SOME <var>:
           here description

       :param ANOTHER <var2>:
           here more descripiton

       describe here whatever :macro:`foo` does

Describe a variable::

    .. var:: MY_VAR

       Describe here whatever :var:`MY_VAR` does.

License
-------

New BSD License.


Author
------

`Kay-Uwe (Kiwi) Lorenz <kiwi@franka.dyndns.org>`_ (http://quelltexter.org)
