# -*- coding: utf-8 -*-
#
# sphinxcontrib-zopeext documentation build configuration file
#
# Based on conf.py from the main sphinx documentation

import os.path
import sys

# So that python can find sphinxcontrib.zopeext.autointerface.
sys.path.insert(0, os.path.abspath('..'))

import sphinxcontrib.zopeext

extensions = ['sphinx.ext.autodoc',
              'sphinx.ext.autosummary',
              'sphinx.ext.intersphinx',
              'sphinx.ext.viewcode',
              'sphinxcontrib.zopeext.autointerface']

master_doc = 'index'
templates_path = ['_templates']
exclude_patterns = ['_build']

source_suffix = '.rst'

# General information about the project.
project = 'sphinxcontrib-zopeext'
author = 'Michael McNeil Forbes'
copyright = '2009-2012, ' + author
version = sphinxcontrib.zopeext.__version__
release = version

html_theme = 'sphinxdoc'
html_style = 'autointerface.css' # Adds some simple syntax highliting
modindex_common_prefix = ['sphinxcontrib.zopeext.']
html_static_path = ['_static']
html_use_opensearch = 'http://packages.python.org/sphinxcontrib-zopeext'
htmlhelp_basename = 'sphinxcontrib-zopeextdoc'

latex_documents = [('index', 'sphinxcontrib-zopeext.tex', 
                    'sphinxcontrib-zopeext Documentation',
                    author, 'manual', 1)]

latex_elements = {
    'fontpkg': '\\usepackage{palatino}',
}
latex_show_urls = 'footnote'

intersphinx_mapping = {
    'sphinx': ('http://sphinx.pocoo.org', None),
    'zope': ('http://docs.zope.org/', None),
    }
