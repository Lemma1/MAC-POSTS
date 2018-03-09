'''
    sphinxcontrib.swf
    ~~~~~~~~~~~~~~~~~

    This module provides :rst:directive:`swf`, which you can use to embed
    flash objects into your documentation.
    
'''
from docutils import nodes
from docutils.parsers.rst import Directive, directives
import os, sys, re, shutil

__version__ = '0.3'

def bool_option(argument):
    return directives.choice(argument, 
        ('yes', 'no', 'true', 'false', '0', '1'))

def quality_option(argument):
    return directives.choice(argument, 
        ('low', 'autolow', 'autohigh', 'medium', 'high', 'best'))

def scale_option(argument):
    return directives.choice(argument, 
        ('default', 'noborder', 'exactfit', 'noscale'))

def align_option(argument):
    return directives.choice(argument, 
        ('l', 'r', 't'))

def salign_option(argument):
    return directives.choice(argument, 
        ('l', 'r', 't', 'tl', 'tr'))

def wmode_option(argument):
    return directives.choice(argument, 
        ('window', 'direct', 'opaque', 'transparent', 'gpu'))

color_re = re.compile('#[A-Fa-f0-9]{6}')
def color_option(argument):
    argument = argument.strip()
    if not color_re.match(argument):
        raise ValueError("color must have form #HHHHHH where H is a hexvalue")
    return argument

def aspectratio_option(argument):
    return directives.choice(argument, ('portrait', 'landscape'))


class swf(nodes.General, nodes.Inline, nodes.Element): pass

# http://helpx.adobe.com/flash/kb/flash-object-embed-tag-attributes.html
FLASH_PARAMS = {
    'width': directives.nonnegative_int,
    'height': directives.nonnegative_int,
    'loop': bool_option,
    'menu': bool_option,
    'width': directives.length_or_percentage_or_unitless,
    'height': directives.length_or_percentage_or_unitless,
    'play': bool_option,
    'quality': quality_option,
    'scale': scale_option,
    'align': align_option,
    'salign': salign_option,
    'wmode': wmode_option,
    'bgcolor': color_option,
    'base': directives.uri,
    'allowFullScreen': bool_option,
    'allowfullscreen': bool_option,
    'fullScreenAspectRatio': aspectratio_option,
    'fullscreenaspectratio': aspectratio_option,
    'flashvars': directives.unchanged
}

class ShockWaveFlash(Directive):
    '''This directive handles flash content.

    Example::
      .. swf:: path/to/file.swf
         :width:
         :height:
         :allowfullscreen: true
         :class:
         :zoom-to-fit: yes

    '''

    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec = dict({
        'class': directives.class_option,
        'zoom-to-fit': bool_option,
        },
        **FLASH_PARAMS)

    has_content = False

    def run(self):
        env = self.state.document.settings.env

        if 'width' not in self.options:
            self.options['width'] = env.config.swf_width_default
        if 'height' not in self.options:
            self.options['height'] = env.config.swf_height_default
        if 'zoom-to-fit' not in self.options:
            self.options['zoom-to-fit'] = env.config.swf_zoom_to_fit_default
        if 'allowfullscreen' not in self.options and \
           'allowFullScreen' not in self.options:
            self.options['allowfullscreen'] = env.config.swf_allowfullscreen_default

        for opt in self.options:
            typ = self.option_spec.get(opt, None)
            if typ is bool_option:
                if self.options[opt].lower() in ('yes', 'true', '1'):
                    self.options[opt] = True
                else:
                    self.options[opt] = False

        if 'allowfullscreen' in self.options:
            self.options['allowFullScreen'] = self.options['allowfullscreen']
            del self.options['allowfullscreen']

        if 'fullscreenaspectratio' in self.options:
            self.options['fullScreenAspecRatio'] = \
                self.options['fullscreenaspecratio']
            del self.options['fullscreenaspecratio']

        reference = directives.uri(self.arguments[0])
        self.options['uri'] = reference
        env.config.swf_flash_files.append(reference)

        return [ swf(rawsource=self.block_text, **self.options) ]

def html_visit_swf(self, node):
    result = ''

    width  = node['width']
    height = node['height']
    src    = self.attval(node['uri'])

    params = ''
    for k in node.attlist():
        if k in FLASH_PARAMS:
            val = node[k]
            if val is True or val is False: val = str(val).lower()
            params += '<param name="%s" value="%s">\n' % (k, self.attval(val))

    classes = list(node['classes'])

    if len(classes):
        result += '<span class="sphinxcontrib-swf %s">'%' '.join(classes)
    else:
        result += '<span class="sphinxcontrib-swf">'

    # zoom-to-fit onload event of object, for now only for non-IE browsers
    zoom_to_fit = ''
    if node['zoom-to-fit']:
        classes.append('swf-zoom-to-fit')

    attrs = ''
    if classes:
        attrs += ' class="%s"'%' '.join(classes)

    result += '<object%s classid="clsid:D27CDB6E-AE6D-11cf-' \
              '96B8-444553540000" width="%s" height="%s">\n'  \
              % (attrs, width, height)

    result += '<param name="src" value="%s">\n' % src
    result += params

    result += '''<!--[if !IE]>-->
          <object 
            type="application/x-shockwave-flash" 
            data="%s"
            width="%s"
            height="%s"
          >''' %(src, width, height)
    result += params
    
    result += '''
                <!--<![endif]-->
                <span>
                    <a href="http://www.adobe.com/go/getflashplayer"><img 
                     src="http://www.adobe.com/images/shared/download_buttons/get_flash_player.gif" alt="Get Adobe Flash player"></a>
                </span>
                <!--[if !IE]>-->
           </object>
           <!--<![endif]-->
           </object>
           </span>'''

    self.body.append(result)

    raise nodes.SkipNode

def on_builder_inited(app):
    app.config.html_static_path.append( os.path.relpath(
          os.path.join(os.path.dirname(__file__), 'javascript'), 
          app.confdir
          ))
    app.config.swf_flash_files[:] = []

def on_html_collect_pages(app):
    for f in app.config.swf_flash_files:
        src = os.path.join(app.srcdir, f)
        if os.path.exists(src):
            shutil.copy(src, os.path.join(app.builder.outdir, f))
    return []
     

def setup(app):
    app.add_config_value('swf_zoom_to_fit_default', 'yes', 'html')
    app.add_config_value('swf_allowfullscreen_default', 'yes', 'html')
    app.add_config_value('swf_width_default', 400, 'html')
    app.add_config_value('swf_height_default', 300, 'html')

    # for internal use
    app.add_config_value('swf_flash_files', [], 'html')

    app.connect('builder-inited', on_builder_inited)
    app.connect('html-collect-pages', on_html_collect_pages)

    sys.stderr.write("path: %s\n" % app.config.html_static_path)
    app.add_javascript('swf_zoom_to_fit.js')


    app.add_directive('swf', ShockWaveFlash)
    app.add_node(swf, html=(html_visit_swf, None))
    
