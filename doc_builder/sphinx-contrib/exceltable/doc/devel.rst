.. _devel:

===========
Development
===========
This section contains some information about module development - in a case you want to contribute to it.
Which is welcome, btw.

.. contents::
   :local:

.. index:: Building


Getting started
===============
To get started with the development, follow the steps:

- Install development-time dependencies::

    pip install nose


Building
========
Build project package with command::

     python setup.py sdist

Releasing
=========
Steps to make a release:

- Increase the version number (exceltable.py and CHANGES)
- Run tox tests::

    pip install tox
    tox

- Build documentation::

    # NOTE: Sphinx-pypi-upload runs only with Python 2
    pip install sphinx-pypi-upload
    python setup.py build_sphinx

- Upload documentation

    python setup.py upload_docs

- Publish app

    python setup.py sdist upload

.. index:: Testing

.. _testing:

Testing
=======
Project uses `nose`_ for unit testing, `coverage`_ for testing coverage reporting and `tox`_
for compliance testing. To execute the tests, run:

- Unittests: ``python setup.py test``
- Compliance: ``tox``

Project repository comes with ready-made configuration for both of the tools, which are used
automatically.


API
====
This section provides some further information about internals of the module:

.. automodule:: sphinxcontrib.exceltable

.. autoclass:: sphinxcontrib.exceltable.ExcelTableDirective

.. autoclass:: sphinxcontrib.exceltable.ExcelTable

.. automethod:: sphinxcontrib.exceltable.ExcelTable.create_table


Licensing
=========
The software is licensed with liberal MIT license, making it suitable for both
commercial and open source usage:

    .. include:: ../LICENSE

