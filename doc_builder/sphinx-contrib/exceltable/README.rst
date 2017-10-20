Module ``sphinxcontrib.exceltable`` is an extension for Sphinx_, that adds support for including Excel spreadsheets, or part
of them, into Sphinx document. See documentation for further information. It has been tested to run on both Python 2.7 and 3.4

Usage::

  My document
  ===========
  The contents of the setup script:

  .. exceltable:: Table caption
     :file: path/to/document.xls
     :header: 1
     :selection: A1:B3

Read complete documentation: http://pythonhosted.org/sphinxcontrib-exceltable/
Report issues: http://bitbucket.org/birkenfeld/sphinx-contrib/

.. _Sphinx: http://sphinx.pocoo.org/
