Change Log
==========
All major changes to `sphinxcontrib-argdoc` are documented here. Version
numbers for the project follow  the conventions described in
`PEP440 <https://www.python.org/dev/peps/pep-0440/>`_ and
`Semantic versioning <http://semver.org/>`_.

[0.1.3] = 2015-07-08
--------------------

Fixed
.....
 - Building under readthedocs now works


[0.1.2] = 2015-07-07
--------------------

Changed
.......
 - Richer error reporting when modules cannot be executed
 - Changes for improved compatibility with readthedocs.org
 

[0.1.1] = 2015-07-07
--------------------

Changed
.......
 - Moved from independent package `argdoc` to `sphinxcontrib` namespace.
   New package name is `sphinxcontrib.argdoc`
 - Numerous doc updates
 - sphinxcontrib-argdoc.readthedocs.org added to README.rst

Added
.....
 - Subcommands of subcommands are now supported
 - Test cases now generate examples for documentation


[0.1.0] = 2015-07-4
-------------------

Changed
.......
  - Subcommands are now given a summary table, and their parsing deferred
    until the main docstring is complete.
  - Added support for argparser epilogs, though visual styling could use
    some work.
  - Added support for multiple prefix characters.
  - Converted format of CHANGELOG, README, and TODO to reStructuredText
    from Markdown


[0.0.4] = 2015-06-19
--------------------

Added
.....
  -  `argdoc_save_rst` config option to export raw RST after argdoc processing
  -  completed Python 3 support
  -  README.md

Changed
.......
  -  Clearer visual formatting
  -  Changed names of internal functions to make more sense

Fixed
......
  -  Argument group description strings are now output.


[0.0.3] = 2015-06-15
--------------------

Added
.....
  -  Quickstart, advanced usage, FAQ, glossary documentation, et c
  -  Export RST of processed docstrings
  -  argdoc-process-docstring event
  -  improved subcommand support for argparsers that have args
     in the main parser and in subcommands

Fixed
.....
  -  Can now handle option+argument+description lines that have
  multiple arguments.


[0.0.2] - 2015-06-09
--------------------

Added
.....
  -  Now handles formatting of argparsers that have subcommands
  -  added sphinx config variable `argdoc_main_func`
  -  made `setup.py`
  -  added this changelog

Changed
.......
  -  Lowered visual hierarchy of section headers for subcommands
     in processed rst output


[0.0.1]
-------

Changed
.......
  -  Converted method from conf.py to a proper Sphinx extension
