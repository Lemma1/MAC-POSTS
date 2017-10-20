# ==================
# ``run_example.py``
# ==================
#
# A script to convert example.rst into latex.
#
# This script shows how to use Docutils and Sphinx.
#
# Change ``TOOL`` to either ``docutils`` or ``sphinx`` 
# to choose which to use for the conversion.
#
# ::

EXAMPLE_FILE = 'example.rst'
TOOL = 'sphinx'

# Allow changing the tool from the commandline.

import sys, os
if len(sys.argv) == 2:
	if sys.argv[1].lower() in ['sphinx', 'docutils']:
		TOOL = sys.argv[1]
	else:
		EXAMPLE_FILE = sys.argv[1]
elif len(sys.argv) == 3:
	for a in sys.argv:
		if a.lower() in ['sphinx', 'docutils']:
			TOOL = a
	else:
		EXAMPLE_FILE = a

# Using Docutils
# ==============
#
# Docutils is simple.  Just import ``py_directive`` before
# calling one of the ``publish_*`` functions from ``docutils.core``.
#
# ::

if TOOL == 'docutils':
	from docutils.core import publish_file
	import sphinxcontrib.py_directive#<- import py_directive and thats it
	publish_file(source_path=EXAMPLE_FILE,destination_path='example.tex',writer_name='latex')

# Using Sphinx
# ============
#
# Using sphinx is just as easy.  Just include ``py_directive`` in
# the extensions in ``conf.py``.  This example doesn't use a ``conf.py``
# but demonstrates how to get the ``py`` directive activated.
#
# ::

elif TOOL == 'sphinx':
	extensions = ["sphinx.ext.pngmath","sphinxcontrib.py_directive"]#<- include  one of the math extensions and py_directive in extensions and thats it

# The rest of this is code to get sphinx to run without the standard conf.py and 
# isn't required in a standard sphinx environment.

	CONFIG_FILENAME ='A_TEMPORARY_BLANK_CONFIG_FILE_FOR_SPHINX.py'
	from sphinx.application import Sphinx
	import sphinx.application
	preamble = r'''
	\makeatletter
	\renewcommand{\maketitle}{{\LARGE { \textbf {\@title}}}} 
	\makeatother
	\definecolor{TitleColor}{rgb}{0,0,0}
	'''
	sphinx.application.CONFIG_FILENAME = CONFIG_FILENAME
	srcdir = '.'
	confdir = '.'
	outdir = '_build'
	doctreedir = os.path.join(outdir,'.doctrees')
	buildername = 'latex'
	status = None
	warning = sys.stderr
	error = sys.stderr
	warnfile = None
	confoverrides = {}
	tags = []
	freshenv = True
	warningiserror = True
	f = open(CONFIG_FILENAME, 'w')
	f.write('\n')
	f.close()
	app = Sphinx(srcdir, confdir, outdir, doctreedir, buildername,
	                     confoverrides, status, warning, freshenv,
	                     warningiserror, tags)
	input_file = EXAMPLE_FILE
	base = os.path.splitext(input_file)[0]
	app.config.master_doc = base
	app.config.latex_documents = [
	          (base, base+'.tex', '',
	       'Py Directive', 'howto'),]
	app.config.latex_elements = {'tableofcontents':'','release':'', 'logo':'',
	              'preamble':preamble,
	              'pointsize' : '10pt'}
	app.config.exclude_patterns = ['README.rst']
	[app.setup_extension(ext) for ext in extensions]
	force_all = True
	filenames = [input_file]
	app.build(force_all, filenames)
	os.remove(CONFIG_FILENAME)

