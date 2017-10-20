# -*- coding: utf-8 -*-
"""
    sphinx_domain_cmake
    ~~~~~~~~~~~~~~~~~~~

    A CMake domain.

    :copyright: 2012 by Kay-Uwe (Kiwi) Lorenz, ModuleWorks GmbH
    :license: BSD, see LICENSE for details.
"""

from sphinxcontrib.domaintools import custom_domain
import re

from docutils import nodes
from sphinx import addnodes
from sphinx.domains import Domain, ObjType
from sphinx.domains.std import GenericObject
from sphinx.locale import l_, _
from sphinx.directives import ObjectDescription
from sphinx.roles import XRefRole
from sphinx.util.nodes import make_refnode
from sphinx.util.compat import Directive

cmake_param_desc_re = re.compile(
    r'([-_a-zA-Z0-9]+)\s+(.*)')


macro_sig_re = re.compile(r'(\w+)\(([^)]*)\)')
_term = r'''
    <\w+>(?:\.\.\.)?
    | 
       \[
       (?: [^\[\]]+
         | (?=\[) \[ [^\[\]]+ \]  # allow one nesting level
       )+
       \]
    |
       \{
       (?:[^{}]+
         | (?=\{) \{ [^{}]+ \}    # allow one nesting level
       )+
       \}(?:\.\.\.)?
'''  
macro_param_re = re.compile(r'''
    %s | (?P<key>[^\s]+)\s+(?P<value>(?:%s)) | (?P<flag>[^\s]+)
    ''' % (_term,_term), re.VERBOSE)

class desc_cmake_argumentlist(nodes.Part, nodes.Inline, nodes.TextElement):
    """Node for a general parameter list."""
    child_text_separator = ' '

def argumentlist_visit(self, node):
    self.visit_desc_parameterlist(node)

def argumentlist_depart(self, node):
    self.depart_desc_parameterlist(node)

def html_argumentlist_visit(self, node):
    self.visit_desc_parameterlist(node)

    if len(node.children) > 3:
        self.body.append('<span class="long-argument-list">')
    else:
        self.body.append('<span class="argument-list">')


def html_argumentlist_depart(self, node):
    self.body.append('</span>')

    self.depart_desc_parameterlist(node)

class desc_cmake_argument(nodes.Part, nodes.Inline, nodes.TextElement):
    """Node for an argument wrapper"""

def argument_visit(self, node):
    pass

def argument_depart(self, node):
    pass

def html_argument_visit(self, node):
    self.body.append('<span class="arg">')

def html_argument_depart(self, node):
    self.body.append("</span>")

#class desc_cmake_argumentlist(addnodes.desc_parameterlist):
    #child_text_separator = " "

# sphinx.HTMLTranslator should be derived from GenericNodeVisitor, but it
# is not
#import sphinx.writers.html as _html
#kjksetattr(_html.


def _get_param_node(m):
    if m.group('key'):
        node = addnodes.desc_parameter()

        key = nodes.strong(m.group('key'), m.group('key'))
        key['classes'].append('arg-key')
        node += key

        node += nodes.Text(" ", " ")
        value = nodes.inline(m.group('value'), m.group('value'))
        value['classes'].append('arg-value')
        node += value

        return node

    elif m.group('flag'):
        node = addnodes.desc_parameter()
        flag = nodes.strong(m.group('flag'), m.group('flag'))
        flag['classes'].append('arg-flag')
        node += flag
        return flag
        
    else:
        return addnodes.desc_parameter(m.group(0), m.group(0))
                

def parse_macro(env, sig, signode):
    #import rpdb2 ; rpdb2.start_embedded_debugger('foo')
    m = macro_sig_re.match(sig)
    if not m:
        signode += addnodes.desc_name(sig, sig)
        return sig
    name, args = m.groups()
    signode += addnodes.desc_name(name, name)
    plist = desc_cmake_argumentlist()
    for m in macro_param_re.finditer(args):
        arg = m.group(0)

        if arg.startswith('['):
            arg = arg[1:-1].strip()
            x = desc_cmake_argument()
            opt = addnodes.desc_optional()
            x += opt
            m = macro_param_re.match(arg)

            assert m is not None, "%s does not match %s" % (arg, macro_param_re.pattern)

            opt += _get_param_node(m)
            plist += x

        # elif arg.startswith('{') choice
        else:
            x = desc_cmake_argument()
            x += _get_param_node(m)
            plist += x

    signode += plist
    return name
    
def setup(app):
    from sphinx.util.docfields import GroupedField
    app.add_node(
        node = desc_cmake_argumentlist,
        html = (html_argumentlist_visit, html_argumentlist_depart),
        latex = (argumentlist_visit, argumentlist_depart),
    )

    app.add_node(
        node = desc_cmake_argument,
        html = (html_argument_visit, html_argument_depart),
        latex = (argument_visit, argument_depart),
        )

    app.add_domain(custom_domain('CMakeDomain',
        name  = 'cmake',
        label = "CMake", 

        elements = dict(
            macro = dict(
                # role = 'xxx' if differs from macro
                # objtype = 'xxx' if differs from macro
                objname       = "CMake Macro",
                indextemplate = "pair: %s; CMake macro",
                parse         = parse_macro,
                fields        = [ 
                    GroupedField('parameter',
                        label = "Parameters",
                        names = [ 'param' ])
                ]
            ),

            var   = dict(
                objname = "CMake Variable",
                indextemplate = "pair: %s; CMake variable"
            ),
        )))

# vim: ts=4 : sw=4 : et
