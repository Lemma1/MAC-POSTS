#!/usr/bin/env python
# coding: utf-8
# basic conf.py for `sphinxcontrib.argdoc` unit test suite
import datetime
import sphinx_rtd_theme
import sphinxcontrib.argdoc

html_theme = 'sphinx_rtd_theme'
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]


argdoc_save_rst = True # tell argdoc to save RST output after processing
argdoc_prefix_chars = "-+"


# -- General configuration ------------------------------------------------

master_doc = 'master_toctree'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.autosummary',
    'sphinx.ext.intersphinx',
    'sphinxcontrib.argdoc',
    'numpydoc', 
]


# sphinx autodoc config -------------------------------------------------------

autodoc_default_flags = [
    "show-inheritance",
    "undoc-members",
    "special-members",
    "private-members",
    "inherited-members",
]
autodoc_member_order = "bysource"


# intersphinx config ------------------------------------------------------------
intersphinx_mapping = { "python" : ("http://docs.python.org",None),
                        }

# other -------------------------------------------------------------------------


# General information about the project.
project = u'argdoc_test'
copyright = u'2015, Joshua G. Dunn'

# Short version number, for |version|
version = str(sphinxcontrib.argdoc.__version__)
# The full version, including alpha/beta/rc tags, for |release|
release = "%s-r%s" % (sphinxcontrib.argdoc.__version__,str(datetime.date.today()).replace("-","_"))


# The suffix of source filenames.
source_suffix = '.rst'

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

