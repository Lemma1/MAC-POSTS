# -*- coding: utf-8 -*-
"""
    sphinxcontrib.googlemaps
    ~~~~~~~~~~~~~~~~~~~~~~~~~

    :copyright: Copyright 2012 by Takeshi KOMIYA
    :license: BSD, see LICENSE for details.
"""

import re
import urllib
import urllib2
from xml.dom import minidom

from docutils import nodes, utils
from docutils.parsers.rst import directives

from sphinx.util.compat import Directive


def spec_float(argument):
    """
    Check for a float argument; raise ``ValueError`` if not.
    (Directive option conversion function.)
    """
    return float(argument)


class googlemaps(nodes.General, nodes.Element):
    pass


class GoogleMapsDirective(Directive):
    """Directive for embedding google-maps"""

    has_content = False
    required_arguments = 0
    optional_arguments = 1
    final_argument_whitespace = True
    option_spec = {
        'latitude': spec_float,
        'longtitude': spec_float,
        'balloon': directives.flag,
        'zoom': directives.nonnegative_int,
    }

    def run(self):
        node = googlemaps()
        if self.arguments:
            node['query'] = " ".join(self.arguments)

        if 'latitude' in self.options:
            node['latitude'] = self.options['latitude']
        if 'longtitude' in self.options:
            node['longtitude'] = self.options['longtitude']

        if 'zoom' in self.options:
            node['zoom'] = self.options['zoom']

        if 'balloon' in self.options:
            node['balloon'] = True

        document = self.state.document
        if 'latitude' in node and 'longtitude' not in node:
            msg = ('googlemaps directive needs both :latitude: and '
                   ':longtitude: options')
            return [document.reporter.warning(msg, line=self.lineno)]
        elif 'latitude' not in node and 'longtitude' in node:
            msg = ('googlemaps directive needs both :latitude: and '
                   ':longtitude: options')
            return [document.reporter.warning(msg, line=self.lineno)]
        elif 'query' in node and 'latitude' in node:
            msg = ('googlemaps directive cannot have both argument and '
                   ':latitude: option')
            return [document.reporter.warning(msg, line=self.lineno)]
        elif 'query' not in node and 'latitude' not in node:
            msg = ('googlemaps directive needs any argument or '
                   ':latitude:/:longtitude: options')
            return [document.reporter.warning(msg, line=self.lineno)]

        return [node]


def visit_googlemaps_node(self, node):
    lang = 'ja'
    params = dict(f='q',
                  hl=lang,
                  t='m',
                  om=0,
                  ie='UTF8',
                  oe='UTF8',
                  output='embed')

    if 'query' in node:
        params['q'] = node['query'].encode('utf-8')
    else:
        params['ll'] = "%f,%f" % (node['latitude'], node['longtitude'])

    if 'zoom' in node:
        params['z'] = str(node['zoom'])

    if 'balloon' not in node:
        params['iwloc'] = 'B'

    baseurl = "https://maps.google.co.jp/maps?"
    iframe = """<iframe width="600" height="350" frameborder="0"
                        scrolling="no" marginheight="0"
                        marginwidth="0" src="%s">
                </iframe>"""

    url = baseurl + urllib.urlencode(params)
    self.body.append(iframe % url)


def depart_googlemaps_node(self, node):
    pass


def setup(app):
    app.add_node(googlemaps,
                 html=(visit_googlemaps_node, depart_googlemaps_node))
    app.add_directive('googlemaps', GoogleMapsDirective)
