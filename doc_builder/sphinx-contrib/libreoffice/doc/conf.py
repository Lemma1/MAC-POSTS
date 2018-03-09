# -*- coding: utf-8 -*-
#
# Sphinx LibreOffice Plugin documentation build configuration file
#

import sys
import os

# -- General configuration ------------------------------------------------

# Extensions
extensions = ['sphinxcontrib.libreoffice']


# The suffix of source filenames.
source_suffix = '.rst'

# The master toctree document.
master_doc = 'index'

# General information about the project.
project = u'Sphinx LibreOffice Plugin'
copyright = u'2014, Gerard Marull-Paretas'

# The version info for the project
version = '0.2'
release = '0.2'

# Exclude patterns
exclude_patterns = ['_build']

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'


# -- Options for HTML output ----------------------------------------------

# The theme
html_theme = 'default'


# -- Options for LaTeX output ---------------------------------------------

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
  ('index', 'SphinxLibreOfficePlugin.tex', 
   u'Sphinx LibreOffice Plugin Documentation',
   u'Gerard Marull-Paretas', 'manual'),
]

