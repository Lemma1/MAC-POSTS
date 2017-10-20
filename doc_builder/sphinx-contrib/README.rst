.. -*- restructuredtext -*-

=========================
README for sphinx-contrib
=========================

This repository contains a collection of Sphinx_ extensions maintained by
their respective authors.  It is not an official part of Sphinx.

.. _Sphinx: http://bitbucket.org/birkenfeld/sphinx


Installing
==========

Use ``setup.py`` in the subdirectory for the individual extension::

   cd dir
   python setup.py build
   sudo python setup.py install


Contributing
============

If you want to add your own extension, please write an e-mail to
georg@python.org and give your bitbucket user name; you will then
get write access to the public repo at
http://bitbucket.org/birkenfeld/sphinx-contrib.

Send wishes, comments, patches, etc. for individual extensions
to their author as given in the module.


List of extensions
==================
.. Note that this will be viewed using the bitbucket web interface ..
.. which supports a subset, but not the full sphinx markup. ..

- aafig: render embedded ASCII art as nice images using aafigure_.
- actdiag: embed activity diagrams by using actdiag_
- adadomain: an extension for Ada support (Sphinx 1.0 needed)
- ansi: parse ANSI color sequences inside documents
- argdoc_: automatically generate documentation for command-line arguments, descriptions, and help text
- astah: embed diagram by using astah_
- autoanysrc: Gather reST documentation from any source files
- autoprogram_: documenting CLI programs
- autorun: Execute code in a ``runblock`` directive.
- blockdiag: embed block diagrams by using blockdiag_
- cacoo: embed diagram from Cacoo_
- cf3domain: a domain for `CFEngine 3`_ policies
- cheader_: The missing ``c:header`` directive for Sphinx's built-in C domain.
- cheeseshop: easily link to PyPI packages
- clearquest: create tables from ClearQuest_ queries.
- cmakedomain_: a domain for CMake_
- coffeedomain: a domain for (auto)documenting CoffeeScript source code.
- confluencebuilder_: build Confluence-supported markup and optionally publish
  to a Confluence server
- context: a builder for ConTeXt.
- disqus_: embed Disqus comments in documents.
- documentedlist_: converts a Python list to a table in the generated documentation.
- doxylink: Link to external Doxygen-generated HTML documentation
- domaintools_: A tool for easy domain creation.
- email: obfuscate email addresses
- erlangdomain: an extension for Erlang support (Sphinx 1.0 needed)
- examplecode: this extension adds support for a multiple language code block
  widget to Sphinx_.
- exceltable_: embed Excel spreadsheets into documents
- feed: an extension for creating syndication feeds and time-based overviews
  from your site content
- findanything_: an extension to add Sublime Text 2-like findanything panels
  to your documentation to find pages, sections and index entries while typing
- gnuplot: produces images using gnuplot_ language.
- googleanalytics: track web visitor statistics by using `Google Analytics`_
- googlechart: embed charts by using `Google Chart`_
- googlemaps: embed maps by using `Google Maps`_
- httpdomain_: a domain for documenting RESTful HTTP APIs.
- hyphenator: client-side hyphenation of HTML using hyphenator_
- imgur_: embed Imgur images, albums, and metadata in documents.
- inlinesyntaxhighlight_: inline syntax highlighting
- lassodomain_: a domain for documenting Lasso_ source code
- libreoffice: an extension to include any drawing supported by LibreOffice (e.g. odg, vsd...).
- lilypond: an extension inserting music scripts from Lilypond_ in PNG format.
- makedomain_: a domain for `GNU Make`_
- matlabdomain_: document MATLAB_ and `GNU Octave`_ code.
- mockautodoc: mock imports.
- mscgen: embed mscgen-formatted MSC (Message Sequence Chart)s.
- napoleon: supports `Google style`_ and `NumPy style`_ docstrings.
- nicoviceo: embed videos from nicovideo
- nwdiag: embed network diagrams by using nwdiag_
- omegat: support tools to collaborate with OmegaT_ (Sphinx 1.1 needed)
- osaka: convert standard Japanese doc to Osaka dialect (it is joke extension)
- paverutils: an alternate integration of Sphinx with Paver_.
- phpdomain: an extension for PHP support
- plantuml: embed UML diagram by using PlantUML_
- py_directive: Execute python code in a ``py`` directive and return a math node.
- rawfiles: copy raw files, like a CNAME.
- requirements: declare requirements wherever you need (e.g. in test
  docstrings), mark statuses and collect them in a single list
- restbuilder: a builder for reST (reStructuredText) files.
- rubydomain: an extension for Ruby support (Sphinx 1.0 needed)
- sadisplay: display SqlAlchemy model sadisplay_
- sdedit: an extension inserting sequence diagram by using Quick Sequence
  Diagram Editor (sdedit_)
- seqdiag: embed sequence diagrams by using seqdiag_
- slide: embed presentation slides on slideshare_ and other sites.
- swf_: embed flash files
- sword: an extension inserting Bible verses from Sword_.
- tikz: draw pictures with the `TikZ/PGF LaTeX package`_.
- traclinks: create TracLinks_ to a Trac_ instance from within Sphinx
- versioning_: Sphinx extension that allows building versioned docs for self-hosting.
- whooshindex: whoosh indexer extension
- youtube: embed videos from YouTube_
- zopeext: provide an ``autointerface`` directive for using `Zope interfaces`_.

.. _aafigure: https://launchpad.net/aafigure

.. _argdoc: http://sphinxcontrib-argdoc.readthedocs.org

.. _astah: http://astah.change-vision.com/

.. _autoprogram: https://pythonhosted.org/sphinxcontrib-autoprogram/

.. _Cacoo: https://cacoo.com/

.. _confluencebuilder: https://github.com/tonybaloney/sphinxcontrib-confluencebuilder

.. _gnuplot: http://www.gnuplot.info/

.. _paver: http://www.blueskyonmars.com/projects/paver/

.. _Sword: http://www.crosswire.org/sword/

.. _Lilypond: http://lilypond.org/web/

.. _sdedit: http://sdedit.sourceforge.net/

.. _Trac: http://trac.edgewall.org

.. _TracLinks: http://trac.edgewall.org/wiki/TracLinks

.. _OmegaT: http://www.omegat.org/

.. _PlantUML: http://plantuml.sourceforge.net/

.. _PyEnchant: http://www.rfk.id.au/software/pyenchant/

.. _sadisplay: http://bitbucket.org/estin/sadisplay/wiki/Home

.. _blockdiag: http://blockdiag.com/

.. _seqdiag: http://blockdiag.com/

.. _actdiag: http://blockdiag.com/

.. _nwdiag: http://blockdiag.com/

.. _Google Analytics: http://www.google.com/analytics/

.. _Google Chart: https://developers.google.com/chart/
.. _Google Maps: http://maps.google.com/

.. _Google style: http://google-styleguide.googlecode.com/svn/trunk/pyguide.html

.. _NumPy style: https://github.com/numpy/numpy/blob/master/doc/HOWTO_DOCUMENT.rst.txt

.. _hyphenator: http://code.google.com/p/hyphenator/

.. _exceltable: http://packages.python.org/sphinxcontrib-exceltable/

.. _YouTube: http://www.youtube.com/

.. _ClearQuest: http://www-01.ibm.com/software/awdtools/clearquest/

.. _Zope interfaces: http://docs.zope.org/zope.interface/README.html

.. _slideshare: http://www.slideshare.net/

.. _TikZ/PGF LaTeX package: http://sourceforge.net/projects/pgf/

.. _MATLAB: http://www.mathworks.com/products/matlab/

.. _GNU Octave: https://www.gnu.org/software/octave/

.. _matlabdomain: https://pypi.python.org/pypi/sphinxcontrib-matlabdomain/

.. _swf: http://bitbucket.org/klorenz/sphinxcontrib-swf

.. _findanything: http://bitbucket.org/klorenz/sphinxcontrib-findanything

.. _cmakedomain: http://bitbucket.org/klorenz/sphinxcontrib-cmakedomain

.. _GNU Make: http://www.gnu.org/software/make/

.. _makedomain: http://bitbucket.org/klorenz/sphinxcontrib-makedomain

.. _inlinesyntaxhighlight: http://sphinxcontrib-inlinesyntaxhighlight.readthedocs.org

.. _CMake: http://cmake.org

.. _domaintools: http://bitbucket.org/klorenz/sphinxcontrib-domaintools

.. _restbuilder: https://pypi.python.org/pypi/sphinxcontrib-restbuilder

.. _lassodomain: https://pythonhosted.org/sphinxcontrib-lassodomain/

.. _Lasso: http://www.lassosoft.com/

.. _httpdomain: https://pythonhosted.org/sphinxcontrib-httpdomain/

.. _cheader: https://pypi.python.org/pypi/sphinxcontrib-cheader

.. _CFEngine 3: http://cfengine.com/

.. _disqus: https://github.com/Robpol86/sphinxcontrib-disqus

.. _imgur: https://github.com/Robpol86/sphinxcontrib-imgur

.. _versioning: https://github.com/Robpol86/sphinxcontrib-versioning

.. _documentedlist: https://github.com/chintal/documentedlist

For contributors
================

When adding or updating your extension, please adhere to these guidelines:

* Use ``make-ext.py`` to set up your extension subdirectory.
* Each extension must be either a submodule or subpackage of the
  ``sphinxcontrib`` package.  The ``sphinxcontrib/__init__.py`` file in your
  package must not be changed.
* Each extension must be listed in this file under "List of extensions" above.
* Each author should be listed in ``AUTHORS`` along with the extension name.
* It would be good to have all extensions BSD licensed; otherwise make a note in
  an ``ext/LICENSE`` file.
* Each extension can maintain a changelog and readme file; these files should
  be called ``ext/CHANGES`` and ``ext/README.rst``, respectively.
