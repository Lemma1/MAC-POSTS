# -*- coding: utf-8 -*-
"""
    sphinxcontrib.libreoffice
    ~~~~~~~~~~~~~~~~~~~~~~~~~

    Render LibreOffice drawings directly into your documentation
    

    See the documentation file for details.

    :author: Gerard Marull-Paretas <gerardmarull@gmail.com>
    :license: BSD, see LICENSE for details

    Inspired by ``sphinxcontrib-gnuplot`` by Vadim Gubergrits.
"""

import sys
import os
import posixpath

from glob import glob
from subprocess import call, PIPE

import shutil
import tempfile

from PIL import Image, ImageOps

from docutils import nodes
from docutils.parsers.rst.directives.images import Figure

from sphinx.util import ensuredir, relative_uri

# Default output formats
DEFAULT_FORMATS = dict(html='png', latex='pdf')


#-------------------------------------------------------------------------------
# Utilities to find LibreOffice installation
#-------------------------------------------------------------------------------

def libreoffice_find():
    """
    Find a LibreOffice installation
    """

    extrapaths = []

    if os.name == 'nt':
        officebinary = 'soffice.exe'

        if 'PROGRAMFILES' in os.environ.keys():
            extrapaths += glob(os.environ['PROGRAMFILES'] + 
                                '\\LibreOffice*')

        if 'PROGRAMFILES(X86)' in os.environ.keys():
            extrapaths += glob(os.environ['PROGRAMFILES(X86)'] + 
                                '\\LibreOffice*')

    elif os.name == 'mac' or sys.platform == 'darwin':
        officebinary = 'soffice'
        extrapaths += ['/Applications/LibreOffice.app/Contents']

    else:
        officebinary = 'soffice'
        extrapaths += glob('/usr/lib*/libreoffice*') + \
                      glob('/opt/libreoffice*') + \
                      glob('/usr/local/libreoffice*') + \
                      glob('/usr/local/lib/libreoffice*')

    for path in extrapaths:
        binary_path = os.path.join(path, 'program', officebinary)
        if os.path.isfile(binary_path):
            return binary_path

    return None




#-------------------------------------------------------------------------------
# The LibreOffice Directive
#-------------------------------------------------------------------------------

def _option_boolean(arg):
    if not arg or not arg.strip():
        return True
    elif arg.strip().lower() in ('no', '0', 'false'):
        return False
    elif arg.strip().lower() in ('yes', '1', 'true'):
        return True
    else:
        raise ValueError('"%s" unknown boolean' % arg)


class LibreOfficeDirective(Figure):
    """
    Directive that processes LibreOffice drawings
    """

    own_option_spec = dict(
        autocrop = _option_boolean,
    )

    option_spec = Figure.option_spec.copy()
    option_spec.update(own_option_spec)
  
    def run(self):
        libreoffice_options = dict([(k,v) for k,v in self.options.items() 
                                       if k in self.own_option_spec])

        (figure_node,) = Figure.run(self)

        if isinstance(figure_node, nodes.system_message):
            return [figure_node]

        figure_node.libreoffice = dict(drawing=self.arguments[0],
                                       options=libreoffice_options)
        
        return [figure_node]


def libreoffice_render(app, doctree):
    """
    Render LibreOffice drawing (doctree-read callback)
    """

    for figure in doctree.traverse(nodes.figure):

        if not hasattr(figure, 'libreoffice'):
            continue

        drawing = figure.libreoffice['drawing']
        options = figure.libreoffice['options']

        # Check for LibreOffice
        if not app.builder.config.libreoffice_binary:
            app.builder.warn('Unable to find a LibreOffice installation')
            figure.replace_self(nodes.literal_block(drawing, drawing))
            continue

        # Update formats with configuration
        format_map = DEFAULT_FORMATS.copy()
        format_map.update(app.builder.config.libreoffice_format)
        
        # Setup paths
        inp_fn_abs = app.builder.env.relfn2path(drawing)[1]
        inp_fn_base, _ = os.path.splitext(os.path.basename(drawing)) 
        out_fext = format_map[app.builder.format]        
        out_fn = '%s.%s' % (inp_fn_base, out_fext)

        if app.builder.format == 'html':    
            imgpath = relative_uri(app.builder.env.docname, '_images')
            out_fn_rel = posixpath.join(imgpath, out_fn)
            out_dir = os.path.join(app.builder.outdir, '_images')
            out_fn_abs = os.path.join(out_dir, out_fn)           
        else:
            if app.builder.format != 'latex':
                app.builder.warn('libreoffice: the builder format %s '
                    'is not officially supported.' % app.builder.format)
            out_fn_rel = out_fn
            out_dir = app.builder.outdir
            out_fn_abs = os.path.join(out_dir, out_fn)
        
        ensuredir(out_dir)
        
        # Run LibreOffice
        # A solution to run LibreOffice when another instance is already 
        # running is to use a unique UserInstallation folder. More info at:
        # https://www.libreoffice.org/bugzilla/show_bug.cgi?id=37531
        
        if os.name != 'nt':
            tmp_folder = tempfile.mkdtemp()
            tmp_path = 'file://' + tmp_folder
        else:
            tmp_path = '$SYSUSERCONFIG/tmp'

        call([app.builder.config.libreoffice_binary, 
                '--headless',
                '-env:UserInstallation=' + tmp_path,
                '--convert-to', out_fext,
                '--outdir', out_dir,
                inp_fn_abs], stdout=PIPE, stderr=PIPE)

        if os.name != 'nt':
            shutil.rmtree(tmp_folder)
        
        # Crop white borders (images only)
        if 'autocrop' in options and out_fext not in ('pdf', 'svg'):
            if options['autocrop']:  
                im = Image.open(out_fn_abs)
                im.load()
     
                im_box = ImageOps.invert(im).getbbox()
                im = im.crop(im_box)

                im.save(out_fn_abs)

        # Get (w, h) - required to make :scale: work without indicating (w, h)
        if out_fext not in ('pdf', 'svg'):
            im = Image.open(out_fn_abs)
            im.load()
            (out_width, out_height) = im.size     

        # Fill image information
        for image in figure.traverse(nodes.image):
            image['uri'] = out_fn_rel
            try:
                image['width'] = str(out_width)
                image['height'] = str(out_height)
            except: pass


def setup(app):
    app.add_directive('libreoffice', LibreOfficeDirective)

    app.connect('doctree-read', libreoffice_render)
    
    app.add_config_value('libreoffice_binary', libreoffice_find(), 'env')
    app.add_config_value('libreoffice_format', DEFAULT_FORMATS, 'env')

