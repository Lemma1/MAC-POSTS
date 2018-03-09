# -*- coding: utf-8 -*-
"""
    sphinxcontrib.cf3domain
    ~~~~~~~~~~~~~~~~~~~~~~~~

    The CFEngine 3 domain.

    :copyright: Copyright 2015 by Jimmy Thrasibule
    :license: BSD, see LICENSE file for details.
"""
import re

from docutils import nodes
from docutils.parsers.rst import directives

from sphinx import addnodes
from sphinx.roles import XRefRole
from sphinx.locale import l_, _
from sphinx.domains import Domain, ObjType
from sphinx.domains.std import Program
from sphinx.directives import ObjectDescription
from sphinx.util.nodes import make_refnode
from sphinx.util.compat import Directive
from sphinx.util.docfields import Field, TypedField, GroupedField


cf3_sig_re = re.compile(
    r'''^(\w+\s+)?          # Type
        (?:([\w-]+):)?      # Namespace
        ([\w\.-]+)\s*       # Identifier
        (?:\((.*)\)|::)?$   # Argument list
    ''',
    re.VERBOSE
)

_SP     = ' '
_CL_MRK = '::'
_FN_SEP = '.'
_NS_SEP = ':'


class CF3Object(ObjectDescription):
    """
    Description of a general CFEngine 3 object.
    """

    doc_field_types = [
        TypedField(
            'parameters',
            label=l_('Parameters'),
            names=('parameter', 'param', 'argument', 'arg'),
            typenames=('paramtype', 'type'),
            typerolename='obj'
        ),
        GroupedField(
            'classes',
            label=l_('Classes'),
            names=('classes', )
        ),
        GroupedField(
            'defined',
            label=l_('Defines'),
            names=('defines', 'sets')
        ),
        Field(
            'type',
            label=l_('Type'),
            names=('type', )
        ),
        Field(
            'returnvalue',
            has_arg=False,
            label=l_('Returns'),
            names=('returns', 'return')
        ),
        Field(
            'returntype',
            has_arg=False,
            label=l_('Return type'),
            names=('rtype', )
        ),
    ]

    def handle_signature(self, sig, signode):
        """
        Transform a CFEngine 3 signature into RST nodes.
        """
        sig_d = {}
        m = cf3_sig_re.match(sig)
        try:
            sig_d['type'], sig_d['ns'], sig_d['id'], sig_d['args'] = m.groups()
        except AttributeError:
            raise ValueError

        sig_d['ns'] = sig_d['ns'] or self.options.get(
            'namespace', self.env.temp_data.get('cf3:namespace')
        )
        fullname = []
        fn_app   = fullname.append
        fn_app(self.objtype + _FN_SEP)
        if self.objtype in ('body', 'bundle'):
            tnode  = addnodes.desc_type('', '')
            tnode += nodes.Text(self.objtype + _SP, self.objtype)

            try:
                sig_d['type'] = sig_d['type'].strip()
                tnode += nodes.Text(sig_d['type'] + _SP, sig_d['type'])
                fn_app(sig_d['type'] + _FN_SEP)
            except AttributeError:
                self.state_machine.reporter.warning(
                    'invalid %s definition: %s' % (self.objtype, sig),
                    line=self.lineno
                )
            signode += tnode

        if sig_d['ns'] and sig_d['ns'] != 'default' \
            and self.env.config.add_module_names:
            signode += addnodes.desc_addname(
                sig_d['ns'], sig_d['ns'] + _NS_SEP
            )
        signode += addnodes.desc_name(sig_d['id'], sig_d['id'])
        fn_app(sig_d['id'])

        signode['namespace'] = sig_d['ns']
        signode['fullname']  = ''.join(fullname)

        if self.objtype == 'class':
            signode += addnodes.desc_name(_CL_MRK, _CL_MRK)
            return ''.join(fullname), sig_d['ns']

        if not sig_d['args'] and self.objtype == 'function':
            signode += addnodes.desc_parameterlist()
            return ''.join(fullname), sig_d['ns']

        if sig_d['args']:
            paramlist = addnodes.desc_parameterlist()
            for arg in sig_d['args'].split(','):
                arg = arg.strip()
                paramlist += addnodes.desc_parameter(arg, arg)
            signode += paramlist

        return ''.join(fullname), sig_d['ns']

    def get_index_text(self, modname, name):
        """
        Return the text for the index entry of the object.
        """
        objtype = self.objtype
        idxname = name[0].split(_FN_SEP)[-1]
        idxtext = []
        idx_app = idxtext.append

        idx_app('%(idxname)s')
        if objtype == 'function':
            idx_app('()')
        if modname == 'default':
            idx_app(' (%(objtype)s)')
        else:
            idx_app(' (%(objtype)s in %(modname)s)')

        return _(''.join(idxtext)) % locals()

    def add_target_and_index(self, name, sig, signode):
        """
        Add cross-reference IDs and entries to self.indexnode, if applicable.
        """
        modname = name[1] or self.options.get(
            'namespace', self.env.temp_data.get('cf3:namespace')
        )
        fullname = (modname and modname + _NS_SEP or '') + name[0]

        if fullname not in self.state.document.ids:
            signode['names'].append(fullname)
            signode['ids'].append(fullname)
            signode['first'] = (not self.names)
            self.state.document.note_explicit_target(signode)

            objects = self.env.domaindata['cf3']['objects']
            if fullname in objects:
                self.state_machine.reporter.warning(
                    'duplicate object description of %s, ' % fullname
                    + 'other instance in '
                    + self.env.doc2path(objects[fullname][0])
                    + ', use :noindex: for one of them',
                    line=self.lineno
                )
            objects[fullname] = (self.env.docname, self.objtype)

        idxtext = self.get_index_text(modname, name)
        if idxtext:
            self.indexnode['entries'].append(
                ('single', idxtext, fullname, '')
            )


class CF3Namespace(Directive):
    has_content = False
    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = False
    option_spec = {
        'synopsis': lambda x: x,
        'noindex': directives.flag,
        'deprecated': directives.flag,
    }

    def run(self):
        modname = self.arguments[0].strip()

        env = self.state.document.settings.env
        env.temp_data['cf3:namespace'] = modname

        ret = []
        if 'noindex' in self.options:
            return ret

        env.domaindata['cf3']['namespaces'][modname] = (
            env.docname, self.options.get('synopsis', ''),
            'deprecated' in self.options,
        )
        targetnode = nodes.target(
            '', '', ids=['namespace-' + modname], ismod=True
        )
        self.state.document.note_explicit_target(targetnode)
        ret.append(targetnode)

        indextext = _('%s (namespace)') % modname
        ret.append(
            addnodes.index(entries=[
                ('single', indextext, 'namespace-' + modname, modname)
            ])
        )

        return ret


class CF3XRefRole(XRefRole):
    """
    Cross-referencing role for the CFEngine 3 domain.
    """
    def process_link(self, env, refnode, has_explicit_title, title, target):
        if not has_explicit_title:
            target = target.lstrip('~')
            target = target.lstrip('-')

            if title[0:1] == '~':
                title = title[1:].split(_NS_SEP)[-1]
            elif title[0:1] == '-':
                title = title[1:].split(_SP)[-1]
            elif refnode['reftype'] in ('body', 'bundle'):
                title = refnode['reftype'] + _SP + title

        return title, target


class CF3Domain(Domain):
    """CFEngine 3 policy domain."""
    name = 'cf3'
    label = 'CFEngine 3'

    object_types = {
        'attribute': ObjType(l_('attribute'), 'attr',    'obj'),
        'body':      ObjType(l_('body'),      'body',    'obj'),
        'bundle':    ObjType(l_('bundle'),    'bundle',  'obj'),
        'class':     ObjType(l_('class'),     'class',   'obj'),
        'component': ObjType(l_('component'), 'comp',    'obj'),
        'function':  ObjType(l_('function'),  'func',    'obj'),
        'namespace': ObjType(l_('namespace'), 'ns',      'obj'),
        'promise':   ObjType(l_('promise'),   'promise', 'obj'),
        'variable':  ObjType(l_('variable'),  'var',     'obj'),
    }

    directives = {
        'attribute': CF3Object,
        'body':      CF3Object,
        'bundle':    CF3Object,
        'class':     CF3Object,
        'component': Program,
        'function':  CF3Object,
        'namespace': CF3Namespace,
        'promise':   CF3Object,
        'variable':  CF3Object,
    }

    roles = {
        'attr':      CF3XRefRole(),
        'body':      CF3XRefRole(),
        'bundle':    CF3XRefRole(),
        'class':     CF3XRefRole(),
        'comp':      CF3XRefRole(),
        'func':      CF3XRefRole(fix_parens=False),
        'ns':        CF3XRefRole(),
        'promise':   CF3XRefRole(),
        'var':       CF3XRefRole(),
    }

    initial_data = {
        'objects':    {},
        'namespaces': {},
    }

    def clear_doc(self, docname):
        for fullname, (fn, _) in list(self.data['objects'].items()):
            if fn == docname:
                del self.data['objects'][fullname]
        for ns, (fn, _, _) in list(self.data['namespaces'].items()):
            if fn == docname:
                del self.data['namespaces'][ns]

    def find_obj(self, env, modname, sig, typ):
        if not sig:
            return []

        modname = modname and modname + ':' or ''
        s_type  = sig['type'] and _FN_SEP + sig['type'] or ''
        s_id    = sig['id'] and _FN_SEP + sig['id'] or ''
        prefix  = '%(modname)s%(typ)s' % locals()
        suffix  = '%(s_type)s%(s_id)s' % locals()

        objects = self.data['objects']
        startswith = (o for o in objects if o.startswith(prefix))
        return tuple(
            (m, objects[m]) for m in startswith if m.endswith(suffix)
        )

    def resolve_xref(self, env, fromdocname, builder,
                     typ, target, node, contnode):
        if typ in ('ns', 'obj'):
            try:
                docname, synopsis, deprecated = self.data['namespace'][target]
            except KeyError:
                return None

            title = '%s%s' % ( synopsis, (deprecated and ' (deprecated)' or ''))
            return make_refnode(
                builder, fromdocname, docname, 'namespace-' + target,
                contnode, title
            )

        sig_d = {}
        m = cf3_sig_re.match(target)
        try:
            sig_d['type'], sig_d['ns'], sig_d['id'], sig_d['args'] = m.groups()
        except AttributeError:
            return None

        try:
            sig_d['type'] = sig_d['type'].strip()
        except AttributeError:
            pass
        modname = sig_d['ns'] or node.get('cf3:namespace', 'default')
        matches = self.find_obj(env, modname, sig_d, typ)
        if not matches:
            return None

        if len(matches) > 1:
            env.warn_node(
                'more than one target found for cross-reference '
                '%r: %s'%(target, ', '.join(m[0] for m in matches))
            )
        name, obj = matches[0]
        if obj:
            return make_refnode(
                builder, fromdocname, obj[0], name, contnode, name
            )

    def get_objects(self):
        for ns, info in self.data['namespaces'].items():
            yield (ns, ns, 'namespace', info[0], 'namespace-' + ns, 0)
        for refname, (docname, typ) in self.data['objects'].items():
            yield (refname, refname, typ, docname, refname, 1)


def setup(app):
    app.add_domain(CF3Domain)
