#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
from docutils import nodes
from docutils.parsers.rst import directives
from sphinx.util.compat import Directive

class codeio(nodes.General, nodes.Element): pass

def visit_codeio_node(self, node):

    p_attrs = {
        'data-height': node['height'],
        'data-theme-id': node['theme_id'],
        'data-slug-hash': node['slug'],
        'data-default-tab': 'result',
        'data-user': node['user'],
        'class': 'codepen'
    }
    self.body.append(self.starttag(node, "p", **p_attrs))
    a_attrs = {
        'href': 'http://codepen.io/seanseansean/pen/%s/' % node['slug']
    }
    self.body.append('See the Pen')
    self.body.append(self.starttag(node, "a", **a_attrs))
    self.body.append(node['title'])
    self.body.append('</a>')
    self.body.append('by %s' % node['user'])
    self.body.append('(')
    a2_attrs = {
        'href': 'http://codepen.io/seanseansean'
    }
    self.body.append(self.starttag(node, "a", **a2_attrs))
    self.body.append('@%s' % node['user'])
    self.body.append('</a>')
    self.body.append(')')
    self.body.append(' on ')
    a3_attrs = {
        'href': 'http://codepen.io'
    }
    self.body.append(self.starttag(node, "a", **a3_attrs))
    self.body.append('CodePen')
    self.body.append('</a>.</p>')
    script_attrs = {
        'async': 'async',
        'src': '//codepen.io/assets/embed/ei.js'
    }
    self.body.append(self.starttag(node, "script", **script_attrs))
    self.body.append('</script>')

def depart_codeio_node(self, node):
    pass

class CodeIo(Directive):
    has_content = True
    required_arguments = 5
    optional_arguments = 0
    final_argument_whitespace = False
    #TODO: make title an optional argument
    #option_spec = {
    #    'height': directives.unchanged,
    #    'theme_id': directives.unchanged,
    #    'slug': directives.unchanged,
    #    'user': directives.unchanged,
    #    'title': directives.unchanged
    #}

    def run(self):
        return [codeio(height=self.arguments[0],
                       theme_id=self.arguments[1],
                       slug=self.arguments[2],
                       user=self.arguments[3],
                       title=self.arguments[4])]

def setup(app):
    app.addnode(codeio, html=(visit_codeio_node, depart_codeio_node))
    app.add_directive("codeio", CodeIo)
