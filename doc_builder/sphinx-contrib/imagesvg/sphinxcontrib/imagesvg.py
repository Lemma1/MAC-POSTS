# -*- coding: utf-8 -*-
"""
    sphinxcontrib.imagesvg
    ~~~~~~~~~~~~~~~~~~~~~~~~~

    Embed SVG images in various ways in your documentation
    See the documentation file for details.

    :author: Theo J.A. de Vries <t.j.a.devries@gmail.com>
    :license: BSD, see LICENSE for details
"""

import posixpath

from docutils import nodes
from docutils.parsers.rst.directives.images import Image

#-------------------------------------------------------------------------------
# The imagesvg node
#-------------------------------------------------------------------------------

class imagesvg(nodes.image):

    def __init__(self, other = None):
        if other:
            self.__dict__ = dict(other.__dict__)

def visit_imagesvg_html(self, node):
    get_html_tag_attributes = _KNOWN_HTML_TAGS[node['tagtype']]
    attrs = get_html_tag_attributes(self, node)
#    attrs = _get_html_object_tag_attributes(self, node)
    self.body.append(self.starttag(node, node['tagtype'], **attrs))
    self.body.append(('</%s>' % node['tagtype']))


def depart_imagesvg_html(self, node):
    pass


def _get_html_object_tag_attributes(self, node):
    olduri = node['uri']
    # rewrite the URI if the environment knows about it
    if olduri in self.builder.images:
        node['uri'] = posixpath.join(self.builder.imgpath,
                                     self.builder.images[olduri])

    attrs = {
        'type': "image/svg+xml",
        'data': node['uri'],
    }
    if 'width' in node:
        attrs['width'] = node['width']
    if 'height' in node:
        attrs['height'] = node['height']
    if 'alt' in node:
        attrs['alt'] = node['alt']
    return attrs


def css(d):
    return "; ".join(sorted("%s: %s" % kv for kv in d.items()))

def _get_html_iframe_tag_attributes(self, node):
    olduri = node['uri']
    # rewrite the URI if the environment knows about it
    if olduri in self.builder.images:
        node['uri'] = posixpath.join(self.builder.imgpath,
                                     self.builder.images[olduri])

    attrs = {
        'src': node['uri'],
    }
    style = {
    }
    styleLength=0
    if 'width' in node:
        style['width'] = node['width']
        styleLength += 1
    if 'height' in node:
        style['height'] = node['height']
        styleLength += 1
    if styleLength > 0:
        attrs['style'] = css(style)
    if 'alt' in node:
        attrs['alt'] = node['alt']
    return attrs


_KNOWN_HTML_TAGS = {
    # TODO: add other html tags
    'object': (_get_html_object_tag_attributes),
    'iframe': (_get_html_iframe_tag_attributes),
    }


def get_tagtype(d, key):
    if key not in d:
        return 'object'
    if not _KNOWN_HTML_TAGS[d[key]]:
        raise ValueError("invalid tagtype %s" % key)
    return d[key]


#-------------------------------------------------------------------------------
# The ImageSVG Directive
#-------------------------------------------------------------------------------

class ImageSVGDirective(Image):
    """
    Directive to insert SVG markup

    Example::

        .. imagesvg:: test.svg
           :alt: The Image
           :width: 500px
           :height: 500px

    """

    own_option_spec = dict(
        tagtype = str,
    )

    option_spec = Image.option_spec.copy()
    option_spec.update(own_option_spec)

    def run(self):
        imagesvg_options = dict([(k,v) for k,v in self.options.items()
                                       if k in self.own_option_spec])

        (image_node,) = Image.run(self)

        if isinstance(image_node, nodes.system_message):
            return [image_node]

        imagesvg_node = imagesvg(image_node)
        imagesvg_node['tagtype'] = get_tagtype(imagesvg_options, 'tagtype')

        return [imagesvg_node]



def setup(app):
    app.add_node(imagesvg,
                 html=(visit_imagesvg_html, depart_imagesvg_html)
                 )
    app.add_directive('imagesvg', ImageSVGDirective)

