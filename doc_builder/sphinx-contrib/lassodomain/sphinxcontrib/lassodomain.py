# -*- coding: utf-8 -*-
"""
    sphinx.domains.lasso
    ~~~~~~~~~~~~~~~~~~~~

    The Lasso domain, based off of the standard Python and JavaScript domains.

    :copyright: Copyright 2013 by Eric Knibbe
    :license: BSD, see LICENSE for details.
"""

from docutils import nodes

from sphinx import addnodes
from sphinx.directives import ObjectDescription
from sphinx.domains import Domain, ObjType
from sphinx.domains.python import _pseudo_parse_arglist
from sphinx.locale import l_, _
from sphinx.roles import XRefRole
from sphinx.util.compat import Directive
from sphinx.util.docfields import Field, TypedField
from sphinx.util.nodes import make_refnode


class SingleGroupedField(Field):
    """A doc field that is grouped; i.e., all fields of that type will be
    transformed into one field with its body being a comma-separated line.  It
    does not have an argument.  Each item can be linked using the given
    *bodyrolename*.  SingleGroupedField should be used for doc fields that can
    occur more than once, but don't require a description.  If *can_collapse* is
    true, this field will revert to a Field if only used once.

    Example::

       :import: trait_first
       :import: trait_queriable
    """
    is_grouped = True

    def __init__(self, name, names=(), label=None, bodyrolename=None,
                 can_collapse=False):
        Field.__init__(self, name, names, label, False, None, bodyrolename)
        self.can_collapse = can_collapse

    def make_field(self, types, domain, items):
        fieldname = nodes.field_name('', self.label)
        if len(items) == 1 and self.can_collapse:
            return Field.make_field(self, types, domain, items[0])
        bodynode = nodes.paragraph()
        for i, (fieldarg, content) in enumerate(items):
            bodynode += nodes.Text(', ') if i else None
            bodynode += self.make_xref(self.bodyrolename, domain,
                                       content[0].astext(), nodes.Text)
        fieldbody = nodes.field_body('', bodynode)
        return nodes.field('', fieldname, fieldbody)


class SingleTypedField(SingleGroupedField):
    """A doc field that occurs once and can contain type information.  It does
    not have an argument.  The type can be linked using the given
    *typerolename*.  Used in this domain to describe return values and types,
    which are specified with :return: and :rtype: and are combined into a single
    field list item.

    Example::

       :return: description of the return value
       :rtype:  optional description of the return type
    """
    is_typed = True

    def __init__(self, name, names=(), typenames=(), label=None,
                 typerolename=None, can_collapse=False):
        SingleGroupedField.__init__(self, name, names, label, None, can_collapse)
        self.typenames = typenames
        self.typerolename = typerolename

    def make_field(self, types, domain, items):
        def handle_item(fieldarg, content):
            par = nodes.paragraph()
            if fieldarg in types:
                par += nodes.Text(' (')
                fieldtype = types.pop(fieldarg)
                if len(fieldtype) == 1 and isinstance(fieldtype[0], nodes.Text):
                    typename = ''.join(n.astext() for n in fieldtype)
                    par += self.make_xref(self.typerolename, domain, typename)
                else:
                    par += fieldtype
                par += nodes.Text(') ')
            par += content
            return par

        fieldname = nodes.field_name('', self.label)
        fieldarg, content = items[0]
        bodynode = handle_item(fieldarg, content)
        fieldbody = nodes.field_body('', bodynode)
        return nodes.field('', fieldname, fieldbody)


class LSObject(ObjectDescription):
    """Description of a Lasso object.
    """
    doc_field_types = [
        # :param name: description
        # :ptype name: typename (optional)
        # - or -
        # :param typename name: description
        TypedField('parameter', names=('param', 'parameter'),
              typenames=('ptype', 'paramtype', 'type'),
              label=l_('Parameters'), typerolename='type', can_collapse=True),
        # :return: description
        # :rtype: typename (optional)
        SingleTypedField('return', names=('return', 'returns'),
              typenames=('rtype', 'returntype'),
              label=l_('Returns'), typerolename='type', can_collapse=True),
        # :author: name <email>
        SingleGroupedField('author', names=('author', 'authors'),
              label=l_('Author'), can_collapse=True),
        # :see: resource
        Field('seealso', names=('see', 'url'),
              label=l_('See also'), has_arg=False),
        # :parent: typename
        Field('parent', names=('parent', 'super'),
              label=l_('Parent type'), has_arg=False, bodyrolename='type'),
        # :import: trait_name
        SingleGroupedField('import', names=('import', 'imports'),
              label=l_('Imports'), bodyrolename='trait', can_collapse=True),
    ]

    def needs_arglist(self):
        """May return true if an empty argument list is to be generated even if
        the document contains none.
        """
        return False

    def get_signature_prefix(self, sig):
        """May return a prefix to put before the object name in the signature.
        """
        return ''

    def get_index_text(self, objectname, name_obj):
        """Return the text for the index entry of the object.
        """
        raise NotImplementedError('must be implemented in subclasses')

    def handle_signature(self, sig, signode):
        """Transform a Lasso signature into RST nodes.
        """
        sig = sig.strip().replace('  ', ' ').replace(' ::', '::').replace(':: ', '::')
        if '(' in sig:
            if ')::' in sig:
                sig, returntype = sig.rsplit('::', 1)
            else:
                returntype = None
            prefix, arglist = sig.split('(', 1)
            prefix = prefix.strip()
            arglist = arglist[:-1].strip().replace(' =', '=').replace('= ', '=')
        else:
            if '::' in sig:
                sig, returntype = sig.rsplit('::', 1)
            else:
                returntype = None
            prefix = sig
            arglist = None
        if '->' in prefix:
            objectprefix, name = prefix.rsplit('->', 1)
            objectprefix += '->'
        else:
            objectprefix = None
            name = prefix

        objectname = self.env.ref_context.get('ls:object')
        if objectprefix:
            fullname = objectprefix + name
        elif objectname:
            fullname = objectname + '->' + name
        else:
            objectname = ''
            fullname = name

        signode['object'] = objectname
        signode['fullname'] = fullname

        sig_prefix = self.get_signature_prefix(sig)
        if sig_prefix:
            signode += addnodes.desc_annotation(sig_prefix, sig_prefix)
        if objectprefix:
            signode += addnodes.desc_addname(objectprefix, objectprefix)

        signode += addnodes.desc_name(name, name)
        if self.needs_arglist():
            if arglist:
                _pseudo_parse_arglist(signode, arglist)
            else:
                signode += addnodes.desc_parameterlist()
            if returntype:
                signode += addnodes.desc_returns(returntype, returntype)
        return fullname, objectprefix

    def add_target_and_index(self, name_obj, sig, signode):
        refname = name_obj[0].lower()
        if refname not in self.state.document.ids:
            signode['names'].append(name_obj[0])
            signode['ids'].append(refname)
            signode['first'] = (not self.names)
            self.state.document.note_explicit_target(signode)
            objects = self.env.domaindata['ls']['objects']
            if refname in objects:
                self.state_machine.reporter.warning(
                    'duplicate object description of %s, ' % refname +
                    'other instance in ' +
                    self.env.doc2path(objects[refname][0]) +
                    ', use :noindex: for one of them',
                    line=self.lineno)
            objects[refname] = self.env.docname, self.objtype

        objectname = self.env.ref_context.get('ls:object')
        indextext = self.get_index_text(objectname, name_obj)
        if indextext:
            self.indexnode['entries'].append(('single', indextext, refname, '', None))

    def before_content(self):
        # needed for automatic qualification of members (reset in subclasses)
        self.objname_set = False

    def after_content(self):
        if self.objname_set:
            self.env.ref_context['ls:object'] = None


class LSDefinition(LSObject):
    """Description of an object definition (type, trait, thread).
    """
    def get_signature_prefix(self, sig):
        return self.objtype + ' '

    def get_index_text(self, objectname, name_obj):
        return _('%s (%s)') % (name_obj[0], self.objtype)

    def before_content(self):
        LSObject.before_content(self)
        if self.names:
            self.env.ref_context['ls:object'] = self.names[0][0]
            self.objname_set = True


class LSTag(LSObject):
    """Description of an object with a signature (method, member).
    """
    def needs_arglist(self):
        return True

    def get_index_text(self, objectname, name_obj):
        name = name_obj[0].split('->')[-1]
        if objectname or name_obj[1]:
            objectname = name_obj[0].split('->')[0]
            return _('%s() (%s member)') % (name, objectname)
        else:
            return _('%s() (method)') % name


class LSTraitTag(LSTag):
    """Description of a tag within a trait (require, provide).
    """
    def get_signature_prefix(self, sig):
        return self.objtype + ' '

    def get_index_text(self, objectname, name_obj):
        name = name_obj[0].split('->')[-1]
        return _('%s() (%s %s)') % (name, objectname, self.objtype)


class LSXRefRole(XRefRole):
    """Provides cross reference links for Lasso objects.
    """
    def process_link(self, env, refnode, has_explicit_title, title, target):
        refnode['ls:object'] = env.ref_context.get('ls:object')
        if not has_explicit_title:
            title = title.lstrip('->')
            target = target.lstrip('~')
            if title[0:1] == '~':
                title = title[1:]
                arrow = title.rfind('->')
                if arrow != -1:
                    title = title[arrow+2:]
        if target[0:2] == '->':
            target = target[2:]
            refnode['refspecific'] = True
        if '(' in target:
            target = target.partition('(')[0]
            if title.endswith('()'):
                title = title[:-2]
        return title, target


class LassoDomain(Domain):
    """Lasso language domain.
    """
    name = 'ls'
    label = 'Lasso'
    object_types = {
        'method':  ObjType(l_('method'),  'meth'),
        'member':  ObjType(l_('member'),  'meth'),
        'provide': ObjType(l_('provide'), 'meth'),
        'require': ObjType(l_('require'), 'meth'),
        'type':    ObjType(l_('type'),    'type'),
        'trait':   ObjType(l_('trait'),   'trait'),
        'thread':  ObjType(l_('thread'),  'thread'),
    }
    directives = {
        'method':  LSTag,
        'member':  LSTag,
        'provide': LSTraitTag,
        'require': LSTraitTag,  # name and signature only
        'type':    LSDefinition,
        'trait':   LSDefinition,
        'thread':  LSDefinition,
    }
    roles = {
        'meth':   LSXRefRole(fix_parens=True),
        'type':   LSXRefRole(),
        'trait':  LSXRefRole(),
        'thread': LSXRefRole(),
    }
    initial_data = {
        'objects': {}, # fullname -> docname, objtype
    }

    def clear_doc(self, docname):
        for fullname, (fn, _) in list(self.data['objects'].items()):
            if fn == docname:
                del self.data['objects'][fullname]

    def merge_domaindata(self, docnames, otherdata):
        # XXX check duplicates
        for fullname, (fn, objtype) in otherdata['objects'].items():
            if fn in docnames:
                self.data['objects'][fullname] = (fn, objtype)

    def find_obj(self, env, obj, name, typ, searchorder=0):
        if name[-2:] == '()':
            name = name[:-2]
        objects = self.data['objects']
        newname = None
        if searchorder == 1:
            if obj and obj + '->' + name in objects:
                newname = obj + '->' + name
            else:
                newname = name
        else:
            if name in objects:
                newname = name
            elif obj and obj + '->' + name in objects:
                newname = obj + '->' + name
        return newname, objects.get(newname)

    def resolve_xref(self, env, fromdocname, builder, typ, target, node,
                     contnode):
        objectname = node.get('ls:object')
        searchorder = node.hasattr('refspecific') and 1 or 0
        name, obj = self.find_obj(env, objectname, target.lower(), typ,
                                  searchorder)
        if not obj:
            return None
        return make_refnode(builder, fromdocname, obj[0], name, contnode, name)

    def resolve_any_xref(self, env, fromdocname, builder, target, node,
                         contnode):
        objectname = node.get('ls:object')
        name, obj = self.find_obj(env, objectname, target.lower(), None, 1)
        if not obj:
            return []
        return [('ls:' + self.role_for_objtype(obj[1]),
                 make_refnode(builder, fromdocname, obj[0],
                              name, contnode, name))]

    def get_objects(self):
        for refname, (docname, type) in self.data['objects'].items():
            yield (refname, refname, type, docname, refname, 1)


def setup(app):
    app.add_domain(LassoDomain)
