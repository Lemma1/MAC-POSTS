GNU Make Domain
===============

This `Sphinx extension`_ provides a `GNU Make`_ `domain`_.

.. _Sphinx extension: http://sphinx-doc.org
.. _GNU Make: http://www.gnu.org/software/make/
.. _domain: http://sphinx-doc.org/domains.html


Installation
------------

::

    pip install sphinxcontrib-makedomain


Enable Extension
----------------

Add ``sphinxcontrib.makedomain`` in ``extensions`` of `conf.py`::

    extensions = [ 'sphinxcontrib.makedomain' ]


Usage
-----

Get ready for make::

    .. highlight:: make

    .. default-domain:: make

Describe a target::

    .. target:: all

       describe here whatever :target:`all` does

Describe a variable::

    .. var:: MY_VAR

       Describe here whatever :var:`MY_VAR` does.

Describe an exported variable::

    .. expvar:: EXPORTED_VAR

       Describe here whatever :expvar:`EXPORTED_VAR` does.

       This would describe::

           export EXPORTED_VAR = some string


License
-------

New BSD License.


Author
------

`Kay-Uwe (Kiwi) Lorenz <kiwi@franka.dyndns.org>`_ (http://quelltexter.org)
